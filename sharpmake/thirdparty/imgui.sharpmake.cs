using System.IO; // For Path.Combine
using System;
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections.Generic;

namespace Inferno
{
    // Represents the project that will be generated by Sharpmake and that contains
    // the sample C++ code.
    [Generate]
    public class IMGUILib : InfernoThridPartyLib
    {
        readonly string IMGUI_ROOT;
        public IMGUILib() : base("IMGUI")
        {
            IMGUI_ROOT = Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY, @"imgui");
            

            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imconfig.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_demo.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_draw.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_internal.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_tables.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_widgets.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imstb_rectpack.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imstb_textedit.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imstb_truetype.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"imgui_widgets.cpp"));

            //backends
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"backends/imgui_impl_dx12.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"backends/imgui_impl_dx12.cpp"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"backends/imgui_impl_glfw.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"backends/imgui_impl_glfw.cpp"));

            //misc
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"misc/cpp/imgui_stdlib.h"));
            SourceFiles.Add(Path.Combine(IMGUI_ROOT, @"misc/cpp/imgui_stdlib.cpp"));

        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(IMGUI_ROOT);
            conf.AddPrivateDependency<GLFWLib>(target);
        }
    }
}