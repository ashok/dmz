#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzRenderPluginParticleEffectsOSG.h"

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgParticle/AccelOperator>
#include <osgParticle/ConnectedParticleSystem>
#include <osgParticle/ConstantRateCounter>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/FluidProgram>
#include <osgParticle/ModularEmitter>
//#include <osgParticle/ModularProgram>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/RadialShooter>
#include <osgParticle/RandomRateCounter>


dmz::RenderPluginParticleEffectsOSG::RenderPluginParticleEffectsOSG (
      const PluginInfo &Info, 
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _core (0),
      _defs (Info, &_log),
      _rc (Info){

   _noParticleEffect.particleEffect = new osg::Group;
   _init (local);
}


dmz::RenderPluginParticleEffectsOSG::~RenderPluginParticleEffectsOSG () {
   
   _noParticleEffect.particleEffect = 0;

}


// Plugin Interface
void
dmz::RenderPluginParticleEffectsOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginParticleEffectsOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { 
      	
      	_core = RenderModuleCoreOSG::cast (PluginPtr); 
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// Object Observer Interface
void
dmz::RenderPluginParticleEffectsOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_core) {

      DefStruct *ds (_lookup_def_struct (Type));
      
      if (ds) {
         
         ObjectStruct *os (new ObjectStruct (*ds));

         if (os && !_objectTable.store (ObjectHandle, os)) { 
            
            delete os; os = 0;
         }
         
         if (os && os->particleEffect.valid ()) {
            
            osg::Group *g (_core->lookup_dynamic_object (ObjectHandle));

            if (g) { 
            
               g->addChild (os->particleEffect.get ());
               _core->get_scene ()->addChild (particleGeode.get ());
            }
         }
      }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::RenderPluginParticleEffectsOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));
   
   if (os && os->particleEffect.valid ()) {

      unsigned int place (0);
      StateStruct *ss (os->Def.stateMap);

      while (ss) {

         if (Value.contains (ss->State)) { 
            place = ss->Place; ss = 0;
         }
         else { 
            ss = ss->next;
         }
      }
      os->particleEffect->setSingleChildOn (place);
   }
}


void
dmz::RenderPluginParticleEffectsOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::RenderPluginParticleEffectsOSG::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


dmz::RenderPluginParticleEffectsOSG::DefStruct *
dmz::RenderPluginParticleEffectsOSG::_lookup_def_struct (const ObjectType &Type) {

   DefStruct *result (_typeTable.lookup (Type.get_handle ()));
   
   if (!result) {
      
      result = _create_def_struct (Type);
      if (result) { 
         
         _typeTable.store (Type.get_handle (), result);
      }
   }

   return result;
}


dmz::RenderPluginParticleEffectsOSG::DefStruct *
dmz::RenderPluginParticleEffectsOSG::_create_def_struct (const ObjectType &Type) {

   DefStruct *result (0);
   ObjectType currentType (Type);
   
   while (!result && currentType) {

      Config particleEffectsList;

      if (currentType.get_config ().lookup_all_config (
            "render.particle-effects", particleEffectsList)) {

         result = new DefStruct;
         if (_defTable.store (currentType.get_handle (), result)) {

            ConfigIterator it;
            Config particleEffects;

            unsigned int place (1);

            StateStruct *currentState (0);

            while (particleEffectsList.get_next_config (it, particleEffects)) {

               Config particleType;
               if (
                     particleEffects.lookup_config ("smoke", particleType) ||
                     particleEffects.lookup_config ("fire", particleType) ||
                     particleEffects.lookup_config ("dust", particleType)
                   ) {
                  
                  Config offset;
                  Mask state;
                  String stateName;

                  const String ResourceName (
                     config_to_string ("resource", particleType));
                  const Boolean NoParticleEffect (
                     config_to_boolean ("none", particleType));
                  const Boolean StateNameFound (
                     particleType.lookup_attribute ("state", stateName));
                  
                  if (StateNameFound) { _defs.lookup_state (stateName, state); }

                  if (!StateNameFound || state) {
                     
                     ParticleEffectStruct *pes = (NoParticleEffect ? &_noParticleEffect :
                        _create_particle_effect (particleType));  
                     
                     if (pes) {
      
                        unsigned int switchPlace (StateNameFound ? place : 0);
                        if (StateNameFound) { place++; }
   
                        if (((switchPlace + 1) > 
                              result->particleEffect->getNumChildren ()) ||
                              !result->particleEffect->getChild (switchPlace)) {
   
                           result->particleEffect->insertChild (switchPlace, 
                              pes->particleEffect.get ());
   
                           if (switchPlace) {
   
                              StateStruct *ss (new StateStruct (switchPlace, state));
   
                              if (currentState) {
   
                                 currentState->next = ss;
                                 currentState = ss;
                              }
                              else { result->stateMap = currentState = ss; }
                           }
                        }
                     }
                  }
               }
            }
         }
         else { delete result; result = 0; }
      }
      currentType.become_parent ();

      if (!result) { result = _defTable.lookup (currentType.get_handle ()); }
   }

   return result;
}


void
dmz::RenderPluginParticleEffectsOSG::_init (Config &local) {
      
   _defaultHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectStateMask);
}


dmz::RenderPluginParticleEffectsOSG::ParticleEffectStruct *
dmz::RenderPluginParticleEffectsOSG::_create_particle_effect (Config &particle_config) {
   
   ParticleEffectStruct *result (0);
   
   Config offset;
   Config config;
   Mask state;
   String stateName;

   const String ResourceName (config_to_string ("resource", particle_config));
   const Boolean NoParticleEffect (config_to_boolean ("none", particle_config));
   const Boolean StateNameFound (particle_config.lookup_attribute ("state", stateName));

   // taking care of particle-system offset config value   
   osg::ref_ptr<osg::MatrixTransform> offsetMatrixTransform = new osg::MatrixTransform;
   offsetMatrixTransform->setDataVariance (osg::Object::DYNAMIC);
   offsetMatrixTransform->setMatrix (osg::Matrix::translate (0,0,0));
   
   osgParticle::Particle particle; 
   particle.setSizeRange (osgParticle::rangef (0.01,10.0)); // meters
   particle.setLifeTime (65); // seconds
   particle.setMass (0.01); // in kilograms
   
   particleSystem = new osgParticle::ParticleSystem;
   
   osg::ref_ptr<osgParticle::ParticleSystemUpdater> particleSystemUpdater = 
      new osgParticle::ParticleSystemUpdater;
   
   osg::ref_ptr<osgParticle::MultiSegmentPlacer> lineSegment = 
      new osgParticle::MultiSegmentPlacer ();
   
   osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter;
   
   osgParticle::RandomRateCounter *randomRateCounter = 
      static_cast<osgParticle::RandomRateCounter *> (emitter->getCounter ());
   
   osg::ref_ptr<osgParticle::RadialShooter> particleRadialShooter = 
      new osgParticle::RadialShooter ();
   
   osg::ref_ptr<osgParticle::AccelOperator> acceleratorOp 
      = new osgParticle::AccelOperator;
    
   // reading offset
   if (particle_config.lookup_config ("offset", config)) {
      
      const Vector Value = config_to_vector (config);
      // assigning the offset
      offsetMatrixTransform->setMatrix (osg::Matrix::translate (
         osg::Vec3 (Value.get_x (),Value.get_y (),Value.get_z ())));
      
      _log.info << "particle offset:" << Value << endl;
   }


   // reading particle size
   if (particle_config.lookup_config ("particle.size", config)) {
      
      const Float32 MaximumSizeValue (config_to_float32 ("maximum", config, 1.0));
      const Float32 MinimumSizeValue (config_to_float32 ("minimum", config, 0.0));
      
      _log.info << "particle maximum size:" << MaximumSizeValue 
         << ", minimum size:" << MinimumSizeValue << endl;
      particle.setSizeRange (osgParticle::rangef (MinimumSizeValue,MaximumSizeValue));
   }
   // reading particle
   if (particle_config.lookup_config ("particle.alpha", config)) {
      
      const Float32 MaximumAlphaValue (config_to_float32 ("maximum", config, 1.0));
      const Float32 MinimumAlphaValue (config_to_float32 ("minimum", config, 0.0));
      
      _log.info << "particle maximum alpha:" << MaximumAlphaValue 
         << ", minimum alpha:" << MinimumAlphaValue << endl;
      particle.setAlphaRange (
         osgParticle::rangef (MinimumAlphaValue, MaximumAlphaValue));
   }   
   
   osg::Vec4 ColorMinimum (0.0, 0.0, 0.0, 0.0);
   osg::Vec4 ColorMaximum (1.0, 1.0, 1.0, 1.0);
   
   if (particle_config.lookup_config ("particle.color", config)) {

      Config colorConfig;
      if (config.lookup_config ("minimum", colorConfig)) {
   
         ColorMinimum = config_to_osg_vec4_color (
            "minimum", 
            config, 
            osg::Vec4 (0.0, 0.0, 0.0, 1.0));
         _log.info << "color-minimum [" << ColorMinimum.x () << ", " << ColorMinimum.y ()
            << ", " << ColorMinimum.z () << ", " << ColorMinimum.a () << "]" << endl;
      }
      if (config.lookup_config ("maximum", colorConfig)) {
   
         ColorMaximum = config_to_osg_vec4_color (
            "maximum", 
            config, 
            osg::Vec4 (1.0, 1.0, 1.0, 0.0));
   
         _log.info << "color-maximum [" << ColorMaximum.x () << ", " << ColorMaximum.y ()
            << ", " << ColorMaximum.z () << ", " << ColorMaximum.a () << "]" << endl;
      }
   }
   
   particle.setColorRange(
      osgParticle::rangev4 (ColorMinimum, ColorMaximum));  
   
   // reading particle
   if (particle_config.lookup_config ("particle.lifetime", config)) {
      
      const Float32 LifetimeValue (config_to_float32 ("seconds", config, 1.0));
      
      _log.info << "particle lifetime (seconds):" << LifetimeValue << endl;
      particle.setLifeTime (LifetimeValue); // seconds
   }
   
   // reading particle
   if (particle_config.lookup_config ("particle.mass", config)) {
      
      const Float32 MassValue (config_to_float32 ("kilograms", config, 1.0));
      
      _log.info << "particle mass (kilograms):" << MassValue << endl;
      particle.setMass (MassValue); // in kilograms
   }

   // reading particle
   if (particle_config.lookup_config ("particle.radius", config)) {
      
      const Float32 RadiusValue (config_to_float32 ("meters", config, 1.0));
      
      _log.info << "particle radius (meters):" << RadiusValue << endl;
      particle.setRadius (RadiusValue); // in meters

      // reading particle
      if ( particle_config.lookup_config ("particle.density", config)) {
         
         const Float32 DensityValue (
            config_to_float32 ("kilograms-per-metercube", 
            config, 1.0));
         
         _log.info << "particle density (kg-per-metercube):" << DensityValue << endl;
         const Float32 MassValue = 
            DensityValue*RadiusValue*RadiusValue*RadiusValue*osg::PI*4.0f/3.0f;
         particle.setMass (MassValue); 
         _log.info << "particle mass (considering density) (kg):" << MassValue << endl;
      }
   }


   // reading particle
   if ( particle_config.lookup_config ("plume.emission", config)) {
      
      const Int32 minimumEmissionValue (config_to_int32 ("minimum", config, 5));
      const Int32 maximumEmissionValue (config_to_int32 ("maximum", config, 10));
      
      _log.info << "particle emission / second (minimum):" << minimumEmissionValue 
         << ", (maximum):" << maximumEmissionValue << endl;
      randomRateCounter->setRateRange (minimumEmissionValue, maximumEmissionValue); 
   }   

   // reading particle
   if ( particle_config.lookup_config ("plume.speed", config)) {
      
      const Int32 distanceValue (config_to_int32 ("meters", config, 5));
      const Int32 timeValue (config_to_int32 ("seconds", config, 10));
      
      _log.info << "particle speed meters / seconds (meters):" << distanceValue 
         << ", (seconds):" << timeValue << endl;
      particleRadialShooter->setInitialSpeedRange (distanceValue, timeValue); 
   }   
   
   // reading particle
   if ( particle_config.lookup_config ("plume.shooter", config)) {
      
      const Float32 thetaMinimumValue (config_to_float32 ("minimum", config, 0.0));
      const Float32 thetaMaximumValue (config_to_float32 ("maximum", config, 0.177));
      
      _log.info << "particle shooter theta (minimum):" << thetaMinimumValue 
         << ", (maximum):" << thetaMaximumValue << endl;
      particleRadialShooter->setThetaRange (thetaMinimumValue, thetaMaximumValue); 
   }      
   
   // reading particle
   if ( particle_config.lookup_config ("plume.gravity", config)) {
      
      const Float32 gravityValue (config_to_float32 ("value", config, -2.0));
      
      _log.info << "particle gravity:" << gravityValue << endl;
      acceleratorOp->setToGravity (gravityValue); 
   }      
   
   // reading particle
   if ( particle_config.lookup_config ("plume.placer", config)) {
      
      Config points;
      if (config.lookup_all_config ("point", points)) {
 
         ConfigIterator it;
         Config point;
    
         Boolean found (points.get_first_config (it, point));
    
         while (found) {
    
            const Vector Value = config_to_vector (point);
            lineSegment->addVertex (osg::Vec3 (
               Value.get_x (),Value.get_y (),Value.get_z ()));
            found = points.get_next_config (it, point);
         }
         _log.info << "particle placer setup completed:" << endl;
      }
   }      
               
   String foundFile (_rc.find_file (ResourceName));
   
   if (foundFile) {

      result = _particleEffectTable.lookup (foundFile);

      if (!result) {

         result = new ParticleEffectStruct;
         result->particleEffect = offsetMatrixTransform.get ();

         if (result->particleEffect.valid ()) {

            _log.info << "Loaded file: " << foundFile << " (" << ResourceName << ")"
               << endl;
            _particleEffectTable.store (foundFile, result);
            
            // Set the attributes 'texture', 'emmisive' and 'lighting'
            particleSystem->setDefaultAttributes (
               foundFile.get_buffer (), false, false);
            
         }
         else {

            delete result;  
            result = 0;
            _log.info << "Failed loading file: " << foundFile << " (" << ResourceName
               << ")" << endl;
         }
      }
      else{
         
         _log.info << "Found particle effect with name: " << foundFile << " (" 
            << ResourceName  << ")" << endl;

         
      }
   }
   else { _log.info << "Failed finding resource: " << ResourceName << endl; }

   particleSystem->setParticleScaleReferenceFrame (
      osgParticle::ParticleSystem::WORLD_COORDINATES);

   particleGeode  = new osg::Geode; 
   particleGeode->addDrawable (particleSystem.get ());
   
   particleSystemUpdater->addParticleSystem (particleSystem.get ());
   result->particleEffect->addChild (particleSystemUpdater.get ());
   particleSystem->setDefaultParticleTemplate (particle);
   
   // Emitter setup
   emitter->setParticleSystem (particleSystem.get ());
   emitter->setPlacer (lineSegment.get ());
   emitter->setShooter (particleRadialShooter.get ());
   result->particleEffect->addChild (emitter.get ());
   
   osg::ref_ptr<osgParticle::FluidProgram> fluidProgram =
      new osgParticle::FluidProgram;
   fluidProgram->setWind (osg::Vec3 (10.0,0,0));

   result->particleEffect->addChild (fluidProgram.get ());
   
   return result;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginParticleEffectsOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginParticleEffectsOSG (Info, local);
}

};

