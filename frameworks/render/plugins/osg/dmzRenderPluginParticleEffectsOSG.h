#ifndef DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesVector.h>

#include <osg/ref_ptr>
#include <osg/Switch>
#include <osgParticle/ParticleEffect>


namespace osgParticle { 
      class Particle; class ExplosionEffect; class ExplosionDebrisEffect;
      class SmokeEffect; class SmokeTrailEffect; class FireEffect;
   }

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginParticleEffectsOSG :
         public Plugin,
         public ObjectObserverUtil {

      public:
         RenderPluginParticleEffectsOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginParticleEffectsOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);


         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);


         virtual void update_object_scale (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
      
         struct ParticleEffectStruct {

            osg::ref_ptr<osg::Group> particleEffect;
         };
         
         struct StateStruct {

            const unsigned int Place;
            const Mask State;

            StateStruct *next;

            StateStruct (const unsigned int ThePlace, const Mask TheState) :
               Place (ThePlace),
               State (TheState),
               next (0) {;}

            ~StateStruct () { if (next) { delete next; next = 0; } }
         };         
         
         struct DefStruct {

            osg::ref_ptr<osg::Switch> particleEffect;
            StateStruct *stateMap;

            DefStruct () : stateMap (0) {

               particleEffect = new osg::Switch;
               particleEffect->setDataVariance (osg::Object::DYNAMIC);
            }

            ~DefStruct () { if (stateMap) { delete stateMap; stateMap = 0; } }
         };         
         
         

         struct ObjectStruct {

            const DefStruct &Def;
            osg::ref_ptr<osg::Switch> particleEffect;

            ObjectStruct (DefStruct &TheDef) : Def (TheDef) {

               if (Def.particleEffect.valid ()) {

                  osg::CopyOp c;
                  particleEffect = (osg::Switch *)Def.particleEffect->clone (c);
               }
               
            }
         };  
         
                  
         DefStruct *_lookup_def_struct (const ObjectType &Type);
         DefStruct *_create_def_struct (const ObjectType &Type);
         
         ParticleEffectStruct *_create_particle_effect (Config &particle_config);
         
         Definitions _defs;
         Resources _rc;
         
               
         void _init (Config &local);
         void _create_particle ();

         Log _log;
         Handle _flagHandle;
         Handle _defaultHandle;
         Handle _hil;
         
         HashTableStringTemplate<ParticleEffectStruct> _particleEffectTable;
         HashTableHandleTemplate<DefStruct> _typeTable;
         HashTableHandleTemplate<DefStruct> _defTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         
         osg::ref_ptr<osg::Geode> _particleGeode;
         osg::ref_ptr<osgParticle::ParticleSystem> _particleSystem;

         RenderModuleCoreOSG *_core;
         ParticleEffectStruct _noParticleEffect;

      private:
         RenderPluginParticleEffectsOSG ();
         RenderPluginParticleEffectsOSG (const RenderPluginParticleEffectsOSG &);
         RenderPluginParticleEffectsOSG &operator= (
            const RenderPluginParticleEffectsOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H
