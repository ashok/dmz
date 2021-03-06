#ifndef DMZ_RUNTIME_DATA_CONVERTERS_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTERS_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL DataConverterString {

      public:
         DataConverterString (const PluginInfo &Info);
         DataConverterString (RuntimeContext *context);
         ~DataConverterString ();

         String to_string (const Data &Value);
         String to_string (const Data *Value);
         Data to_data (const String &Value);
         Data to_data (const String *Value);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         DataConverterString ();
         DataConverterString (const DataConverterString &);
         DataConverterString &operator= (const DataConverterString &);
   };

   class DMZ_KERNEL_LINK_SYMBOL DataConverterHandle {

      public:
         DataConverterHandle (const PluginInfo &Info);
         DataConverterHandle (RuntimeContext *context);
         ~DataConverterHandle ();

         Handle to_handle (const Data &Value);
         Handle to_handle (const Data *Value);
         Data to_data (const Handle &Value);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         DataConverterHandle ();
         DataConverterHandle (const DataConverterHandle &);
         DataConverterHandle &operator= (const DataConverterHandle &);
   };

};

#endif // DMZ_RUNTIME_DATA_CONVERTERS_DOT_H
