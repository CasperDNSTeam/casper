#include "casper_client.h"

#include <config.h>

#include <iostream>
#include <QDebug>
#include <QString>
#include <QSettings>

#define YES 1
#define NO 0
//#include <objbase.h>
#include <atlbase.h>
//#include <iostream>
#include <wbemidl.h>
#include <comutil.h>

namespace Casper {
Client *Client::__self = 0;

typedef QList<IWbemClassObject *> AdapterList;
class Client::ClientContext : public IWbemObjectSink{
public:
  ClientContext() {}
  ~ClientContext() {}

  void init();

  QString GetObjectProperyValue(IWbemClassObject* result_object,
                                           LPCWSTR key);
  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();
  virtual HRESULT STDMETHODCALLTYPE
     QueryInterface(REFIID riid, void** ppv);

  virtual HRESULT STDMETHODCALLTYPE Indicate(
          /* [in] */
          LONG lObjectCount,
          /* [size_is][in] */
          IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
          );

  virtual HRESULT STDMETHODCALLTYPE SetStatus(
          /* [in] */ LONG lFlags,
          /* [in] */ HRESULT hResult,
          /* [in] */ BSTR strParam,
          /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
          );
  LONG m_lRef;
  CComPtr<IWbemServices> m_wmi_query_service;
  void set_dns_search_order(const QStringList &dns_server_search_order,
                            IWbemClassObject *m_wmi_config_object);
  AdapterList ActiveService();
  void ClearAdapterList(AdapterList list);
};


Client *Client::Get() {
  if (__self)
    return __self;

  __self = new Client();

  return __self;
}

Client::Client(QObject *parent) : QObject(parent), ctx(new ClientContext) {}

Client::~Client() { delete ctx; }

void Client::ClientContext::ClearAdapterList(AdapterList list)
{
    foreach(IWbemClassObject *obj, list) {
        obj->Release();
    }
    list.clear();
}

Boolean Client::InitConnection() {
  ctx->init();
  AdapterList list = ctx->ActiveService();

  ctx->ClearAdapterList(list);

  return YES;
}

Boolean Client::InitHandShake() {
  if (!InitConnection()) {
    return NO;
  }
  
  emit ClientState(kClientConnected);
  return YES;
}

Boolean Client::GetAccess() {
  Boolean result = NO;
  return result;
}

void Client::RollbackDNS() {
    AdapterList list = ctx->ActiveService();
    QStringList dns_list = QStringList();

    foreach(IWbemClassObject *obj, list) {
      ctx->set_dns_search_order(dns_list, obj);
    }

    ctx->ClearAdapterList(list);
}

void Client::UpdateDNS(const QString &_IN_primary,
                       const QString &_IN_secondry) {
    AdapterList list = ctx->ActiveService();
    QStringList dns_list;
    dns_list << _IN_primary;
    dns_list << _IN_secondry;

    foreach(IWbemClassObject *obj, list) {
      ctx->set_dns_search_order(dns_list, obj);
    }
    ctx->ClearAdapterList(list);
}

void Client::ClientContext::init()
{
    HRESULT result = CoInitializeSecurity( NULL,
                                           -1,
                                           NULL,
                                           NULL,
                                           RPC_C_AUTHN_LEVEL_CONNECT,
                                           RPC_C_IMP_LEVEL_IMPERSONATE,
                                           NULL,
                                           EOAC_NONE,
                                           NULL );
    if (!SUCCEEDED(result)) {
        if (result == RPC_E_TOO_LATE) {
            CComPtr<IWbemLocator> _com_locator_ref;
            result = CoCreateInstance( CLSID_WbemAdministrativeLocator, NULL,
                                       CLSCTX_INPROC_SERVER,
                                       IID_IWbemLocator,
                                       reinterpret_cast< void** >( &_com_locator_ref ) );
            if (!SUCCEEDED(result)) {
                qFatal( "[dns_client] [windows] - Failed Elevate Permissions");
                return;
            }

            result = _com_locator_ref->ConnectServer(L"root\\cimv2", NULL, NULL, NULL,
                                            WBEM_FLAG_CONNECT_USE_MAX_WAIT,
                                            NULL, NULL, &m_wmi_query_service);
            if (!SUCCEEDED(result)) {
                qWarning()  << "[dns_client] [windows] [warning] - "
                               "Connecting to Windows Service";
            }
        }
        return;
    }

    CComPtr<IWbemLocator> locator;
    result = CoCreateInstance( CLSID_WbemAdministrativeLocator, NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IWbemLocator, reinterpret_cast< void** >( &locator ) );
    if (FAILED(result)) {
        qFatal("[dns_client] [windows] - Error Failed to initiate Service Client");
        return;
    }

    result = locator->ConnectServer(L"root\\cimv2", NULL, NULL, NULL,
                                 WBEM_FLAG_CONNECT_USE_MAX_WAIT,
                                 NULL, NULL, &m_wmi_query_service);
    if (!SUCCEEDED(result)) {
        qFatal("[dns_client] [windows] - Error Failed to initiate Service Client");
    }

    qDebug() << "[dns_client] [windows] - Sucess Connecting to Windows Service";
}

QString Client::ClientContext::GetObjectProperyValue(
        IWbemClassObject *result_object, LPCWSTR key)
{
    QString result;
    CIMTYPE variantType;
    VARIANT vtProp;

    if (!result_object)
        return result;

    HRESULT object_result = result_object->Get(key, 0, &vtProp, &variantType, 0);

    if (!SUCCEEDED(object_result)) {
        qDebug() << Q_FUNC_INFO << "Object Query Failed";
        return result;
    }

    if (vtProp.vt != VT_NULL && vtProp.vt != VT_EMPTY && vtProp.vt == VT_BSTR) {
        BSTR bstr = vtProp.bstrVal;
        QString str((QChar*)bstr,::SysStringLen(bstr));
        result = str;
        SysFreeString(bstr);
    }

    return result;
}

ULONG Client::ClientContext::AddRef() {
  return InterlockedIncrement(&m_lRef);
}

ULONG Client::ClientContext::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT Client::ClientContext::QueryInterface(const IID &riid, void **ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }

    return E_NOINTERFACE;
}

HRESULT Client::ClientContext::Indicate(LONG lObjectCount,
                                        IWbemClassObject **apObjArray) {
 return WBEM_S_NO_ERROR;
}

HRESULT Client::ClientContext::SetStatus(LONG lFlags,
                                         HRESULT hResult,
                                         BSTR strParam,
                                         IWbemClassObject *pObjParam) {
 return WBEM_S_NO_ERROR;
}

static BSTR B_STR(const QString &qstring)
{
        WCHAR* const pBuffer = new WCHAR[qstring.size()+1];
        //large enough to hold the string plus null terminator
        qstring.toWCharArray(pBuffer);
        pBuffer[qstring.size()]=0;//terminate with 0
        BSTR result = SysAllocString(pBuffer);
        delete [] pBuffer;
        return result;
}

AdapterList Client::ClientContext::ActiveService() {
    AdapterList rv;

    CComPtr<IEnumWbemClassObject> enumerator;
    HRESULT result = m_wmi_query_service->ExecQuery( L"WQL",
                                                     L"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = True",
                                                     WBEM_FLAG_FORWARD_ONLY, NULL, &enumerator);
    if (SUCCEEDED(result)){
        while(result == WBEM_S_NO_ERROR) {
            ULONG result_count;
            IWbemClassObject* result_objects[10];
            result = enumerator->Next(WBEM_INFINITE, 10, result_objects, &result_count );
            if (SUCCEEDED(result)) {
                // Do something with the objects.
                //ProcessObjects( uReturned,  apObj );
                for (ULONG n = 0; n < result_count; n++) {
                    //get property names.
                    SAFEARRAY *propery_names = NULL;
                    HRESULT object_result = result_objects[n]->GetNames(NULL,
                                                                        WBEM_FLAG_ALWAYS,
                                                                        NULL,
                                                                        &propery_names);

                    if (SUCCEEDED(object_result)) {
#if 0
                        long lLower, lUpper;
                        BSTR PropName = NULL;
                        SafeArrayGetLBound(propery_names, 1, &lLower);
                        SafeArrayGetUBound(propery_names, 1, &lUpper);

                        for (long i = lLower; i <= lUpper; i++) {
                            // Get this property.
                            object_result = SafeArrayGetElement(
                                        propery_names,
                                        &i,
                                        &PropName);

                            QString str((QChar*)PropName,::SysStringLen(PropName));
                            qDebug() << Q_FUNC_INFO << str <<  " :" <<
                                        GetObjectProperyValue(result_objects[n], PropName);

                            SysFreeString(PropName);
                        }
#endif

                        result_objects[n]->AddRef();
                        rv.append(result_objects[n]);
                    }

                    SafeArrayDestroy(propery_names);
                    //result_objects[n]->Release();
                }
            }
        }
    } else {
       qDebug() << Q_FUNC_INFO << "Failed to Query Win32 Network Configuration";
    }

    qDebug() << "[dns_client] [windows] - Active Adapters Count :"
             << rv.count();
    return rv;
}

void Client::ClientContext::set_dns_search_order(
        const QStringList &dns_server_search_order,
        IWbemClassObject *m_wmi_config_object) {
   bool is_dns_reset = false;
   if (dns_server_search_order.count() < 1)  {
       is_dns_reset = true;
   }

    HRESULT error_state;
    VARIANT ip_list_v;
    SAFEARRAY *array = 0;

    array = SafeArrayCreateVector(VT_BSTR, 0, dns_server_search_order.count());

    for(int i = 0; i < dns_server_search_order.count(); i++) {

        BSTR ip_address_1 = SysAllocString(B_STR(dns_server_search_order.at(i)));
        long idx_1 = i;
        error_state = SafeArrayPutElement(array, &idx_1, ip_address_1);

        if (FAILED(error_state)) {
            SafeArrayDestroy(array);
            qDebug() << Q_FUNC_INFO << "Basic Error : Error Allocating String Array";
            return;
        }

    }

    VariantInit(&ip_list_v);
    V_VT(&ip_list_v) = VT_ARRAY|VT_BSTR;
    V_ARRAY(&ip_list_v) = array;

    IWbemClassObject *method_instence = 0;
    IWbemClassObject *writable_object = 0;
    IWbemClassObject *method_argument_object = 0;
    BSTR method_name = SysAllocString(L"SetDNSServerSearchOrder");

    error_state =
            m_wmi_query_service->GetObject(
                L"Win32_NetWorkAdapterConfiguration",
                0, NULL,&writable_object, NULL);

    if (!SUCCEEDED(error_state)) {
        qDebug() << Q_FUNC_INFO << "Failed horrible way";
        return;
    }

    error_state = writable_object->GetMethod(method_name, 0, &method_instence, NULL);
    if (!SUCCEEDED(error_state)) {
        qDebug() << Q_FUNC_INFO
                 << "COM Error : Failed to Get Method from wmi_config object";
        return;
    }

    error_state = method_instence->SpawnInstance(0, &method_argument_object);

    if (!SUCCEEDED(error_state)) {
        qDebug() << Q_FUNC_INFO
                 << "COM Error : Failed to Get Method Argument Error";
        return;
    }

    if (!m_wmi_config_object) {
        qCritical() << Q_FUNC_INFO
                    << "Rquesting DNS operation on Invalid Query Object Instance...............[ Error] ";
        return;
    }

    error_state = method_argument_object->Put(L"DNSServerSearchOrder", 0, &ip_list_v, 0);
    if (!SUCCEEDED(error_state)) {
        qDebug() << Q_FUNC_INFO
                 << "COM Error : Failed to Get Method Argument Value Error";
        return;
    }

    VARIANT vtProp;
    CIMTYPE variantType;

    HRESULT object_result = m_wmi_config_object->Get(L"__RELPATH", 0L, &vtProp, &variantType, 0);
    if (SUCCEEDED(object_result)) {
        if (vtProp.vt != VT_NULL && vtProp.vt != VT_EMPTY && vtProp.vt == VT_BSTR) {
            BSTR bstr = vtProp.bstrVal;
            //QString str((QChar*)bstr,::SysStringLen(bstr));
            IWbemClassObject *output = 0;
            IErrorInfo *error_info = 0;
            error_state = m_wmi_query_service->ExecMethod(
                        bstr, method_name, 0, NULL, method_argument_object, &output, NULL);
            if (!SUCCEEDED(error_state)) {
                GetErrorInfo(0, &error_info);
                BSTR error_details;
                error_info->GetDescription(&error_details);

                QString str_detail((QChar*)error_details,::SysStringLen(error_details));

                qDebug() << Q_FUNC_INFO << "Failed to Set DNS" << str_detail;
                return;
            }

            VARIANT v_ret_value;
            VariantInit(&v_ret_value);
            error_state = output->Get(L"ReturnValue", 0, &v_ret_value, 0, 0);
            if (!SUCCEEDED(error_state)) {
                qDebug() << Q_FUNC_INFO << "Error status Unknown";
                return;
            }

            error_state = V_I4(&v_ret_value);
            if(error_state != 0) {
                qDebug() << Q_FUNC_INFO << "EnableDNS Failed" << error_state;
            } else {
                qDebug() << "Adapter: " << "EnableDNS..............[ok]";
            }
        }
    }
}

/*
void Client::ClientContext::SyncCurrentProfile(SCNetworkServiceRef service) {
  qDebug() << Q_FUNC_INFO << "Save settings - ";
  QSettings settings("org.caspersecurity.profilecache", "v1.0.0");

  CFArrayRef dns_servers = osx_system_service_get_dns_of_service(service);
  bool _dhcp = osx_system_service_service_uses_dhcp(service);

  settings.beginGroup(
      QString::fromCFString(osx_system_service_get_service_id(service)));
  settings.setValue("dhcp", _dhcp);

  if (service && dns_servers) {
    settings.setValue("server", CFArrayToQStringList(dns_servers));
  }
  settings.endGroup();

  settings.sync();
}

QStringList Client::ClientContext::CFArrayToQStringList(CFArrayRef value) {
  QStringList result;

  if (!value)
    return result;

  for (int i = 0; i < CFArrayGetCount(value); i++) {
    result << QString::fromCFString(
        (CFStringRef)CFArrayGetValueAtIndex(value, i));
  }

  return result;
}
*/

}
