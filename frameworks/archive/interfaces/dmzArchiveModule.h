#ifndef DMZ_ARCHIVE_MODULE_DOT_H
#define DMZ_ARCHIVE_MODULE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

namespace dmz {

   //! \cond
   const char ArchiveModuleInterfaceName[] = "ArchiveModuleInterface";
   //! \endcond

   //! \brief Default archive group name.
   //! \ingroup Archive
   const char ArchiveDefaultName[] = "Default_Archive";

   class ArchiveObserver;
   class Config;

   class ArchiveModule {

      public:
         static ArchiveModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         // ArchiveModule Interface
         virtual Boolean register_archive_observer (
            const Handle ArchiveHandle,
            ArchiveObserver &observer) = 0;

         virtual Boolean release_archive_observer (
            const Handle ArchiveHandle,
            ArchiveObserver &observer) = 0;

         virtual Config create_archive (const Handle ArchiveHandle) = 0;
         virtual void process_archive (const Handle ArchiveHandle, Config &archive) = 0;

      protected:
         ArchiveModule (const PluginInfo &Info);
         ~ArchiveModule ();

      private:
         ArchiveModule ();
         ArchiveModule (const ArchiveModule &);
         ArchiveModule &operator= (const ArchiveModule &);

         const PluginInfo &__Info;
   };
};


inline dmz::ArchiveModule *
dmz::ArchiveModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ArchiveModule *)lookup_rtti_interface (
      ArchiveModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ArchiveModule::ArchiveModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ArchiveModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::ArchiveModule::~ArchiveModule () {

   remove_rtti_interface (ArchiveModuleInterfaceName, __Info);
}

#endif // DMZ_ARCHIVE_MODULE_DOT_H
