require "lmkbuild"

local ipairs = ipairs
local print = print
local sys = lmkbuild.system ()
local lmk = lmk

module (...)

function set_name (name)
   lmk.set_name (name, { DMZ_USE_OSG = true, })
end

function add_libs (list)
   if sys == "win32" then
      lmk.add_vars {localDefines = "$(lmk.defineFlag)WIN32"}
   end
   if sys == "macos" then
      local paths = "-Xlinker -executable_path -Xlinker " ..
         "$(DMZ_OSG_LIB_PATH) -F$(DMZ_OSG_LIB_PATH)"
      for ix, element in ipairs (list) do
         paths = paths .. " -framework " .. element
      end
      lmk.add_vars {
         localIncludes = "-F$(DMZ_OSG_INCLUDE_PATH)",
         localLibPaths = paths,
      }
   else
      local libs = nil
      for ix, element in ipairs (list) do
         if libs then libs = libs .. " "
         else libs = ""
         end
         libs = libs .. "$(lmk.libPrefix)" .. element ..
            "$(DMZ_OSG_DEBUG)$(lmk.libSuffix)"
      end
      lmk.add_vars {
         localIncludes = "$(lmk.includePathFlag)$(DMZ_OSG_INCLUDE_PATH)",
         localLibPaths = "$(lmk.libPathFlag)$(DMZ_OSG_LIB_PATH)",
         localLibs = libs,
      }
   end
end
