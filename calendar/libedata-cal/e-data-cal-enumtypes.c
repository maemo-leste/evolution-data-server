


#include <e-data-cal-types.h>
#include "e-data-cal-enumtypes.h"

/* enumerations from "e-data-cal-types.h" */
GType
e_data_cal_call_status_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { Success, "Success", "success" },
      { Busy, "Busy", "busy" },
      { RepositoryOffline, "RepositoryOffline", "repositoryoffline" },
      { PermissionDenied, "PermissionDenied", "permissiondenied" },
      { InvalidRange, "InvalidRange", "invalidrange" },
      { ObjectNotFound, "ObjectNotFound", "objectnotfound" },
      { InvalidObject, "InvalidObject", "invalidobject" },
      { ObjectIdAlreadyExists, "ObjectIdAlreadyExists", "objectidalreadyexists" },
      { AuthenticationFailed, "AuthenticationFailed", "authenticationfailed" },
      { AuthenticationRequired, "AuthenticationRequired", "authenticationrequired" },
      { UnsupportedField, "UnsupportedField", "unsupportedfield" },
      { UnsupportedMethod, "UnsupportedMethod", "unsupportedmethod" },
      { UnsupportedAuthenticationMethod, "UnsupportedAuthenticationMethod", "unsupportedauthenticationmethod" },
      { TLSNotAvailable, "TLSNotAvailable", "tlsnotavailable" },
      { NoSuchCal, "NoSuchCal", "nosuchcal" },
      { UnknownUser, "UnknownUser", "unknownuser" },
      { OfflineUnavailable, "OfflineUnavailable", "offlineunavailable" },
      { SearchSizeLimitExceeded, "SearchSizeLimitExceeded", "searchsizelimitexceeded" },
      { SearchTimeLimitExceeded, "SearchTimeLimitExceeded", "searchtimelimitexceeded" },
      { InvalidQuery, "InvalidQuery", "invalidquery" },
      { QueryRefused, "QueryRefused", "queryrefused" },
      { CouldNotCancel, "CouldNotCancel", "couldnotcancel" },
      { OtherError, "OtherError", "othererror" },
      { InvalidServerVersion, "InvalidServerVersion", "invalidserverversion" },
      { InvalidArg, "InvalidArg", "invalidarg" },
      { NotSupported, "NotSupported", "notsupported" },
      { NotOpened, "NotOpened", "notopened" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("EDataCalCallStatus", values);
  }
  return etype;
}
GType
e_data_cal_obj_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GFlagsValue values[] = {
      { Event, "Event", "event" },
      { Todo, "Todo", "todo" },
      { Journal, "Journal", "journal" },
      { AnyType, "AnyType", "anytype" },
      { 0, NULL, NULL }
    };
    etype = g_flags_register_static ("EDataCalObjType", values);
  }
  return etype;
}
GType
e_data_cal_obj_mod_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GFlagsValue values[] = {
      { This, "This", "this" },
      { ThisAndPrior, "ThisAndPrior", "thisandprior" },
      { ThisAndFuture, "ThisAndFuture", "thisandfuture" },
      { All, "All", "all" },
      { 0, NULL, NULL }
    };
    etype = g_flags_register_static ("EDataCalObjModType", values);
  }
  return etype;
}



