using System.IO; // For Path.Combine
using Sharpmake;

namespace Inferno
{
    namespace Defines
    {
        public class Paths
        {
            static public readonly string SHARPMAKE_EXE_DIR = System.Reflection.Assembly.GetEntryAssembly().Location;
            
            static public readonly string INFERNO_ROOT = Path.Combine(Defines.Paths.SHARPMAKE_EXE_DIR, @"..\..\..");
            static public readonly string INFERNO_PROJECTS = Path.Combine(INFERNO_ROOT, @"projects");
            static public readonly string INFERNO_ENGINE = Path.Combine(INFERNO_ROOT, @"engine");
             static public readonly string INFERNO_DATA = Path.Combine(INFERNO_ROOT, @"data");

            static public readonly string INFERNO_THIRD_PARTY = Path.Combine(INFERNO_ROOT, @"thirdparty\src");

            static public readonly string INFERNO_CORE = Path.Combine(INFERNO_ENGINE, @"core\");
            static public readonly string INFERNO_CORE_SRC = Path.Combine(INFERNO_ENGINE, @"core\src");
            static public readonly string INFERNO_CORE_INC = Path.Combine(INFERNO_ENGINE, @"core\include");

            static public readonly string INFERNO_GRAPHICS = Path.Combine(INFERNO_ENGINE, @"graphics\");
            static public readonly string INFERNO_GRAPHICS_SRC = Path.Combine(INFERNO_ENGINE, @"graphics\src");
            static public readonly string INFERNO_GRAPHICS_INC = Path.Combine(INFERNO_ENGINE, @"graphics\include");

            static public readonly string INFERNO_EDITOR = Path.Combine(INFERNO_ENGINE, @"editor\");
            static public readonly string INFERNO_EDITOR_SRC = Path.Combine(INFERNO_ENGINE, @"editor\src");
            static public readonly string INFERNO_EDITOR_INC = Path.Combine(INFERNO_ENGINE, @"editor\include");
        } 
    }

    abstract public class InfernoApp : InfernoProject
    {
        public InfernoApp(string projectName) : base(projectName, Project.Configuration.OutputType.Exe)
        {

        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_ENGINE, @"pch"));
            conf.PrecompHeader = Path.Combine(@"infPCH.h");
            conf.PrecompSource = Path.Combine(@"infPCH.cpp");
        }
    }

    abstract public class InfernoLib : InfernoProject
    {
        public InfernoLib(string projectName) : base(projectName, Project.Configuration.OutputType.Lib)
        {

        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_ENGINE, @"pch"));
            conf.PrecompHeader = Path.Combine(@"infPCH.h");
            conf.PrecompSource = Path.Combine(@"infPCH.cpp");
        }
    }

    abstract public class InfernoThridPartyLib : InfernoProject
    {
        public InfernoThridPartyLib(string projectName) : base(projectName, Project.Configuration.OutputType.Lib)
        {

        }
    }

    abstract public class InfernoProject : Project
    {
        //public readonly string ProjectName;
        public readonly Project.Configuration.OutputType ProjectType;

        public InfernoProject(string projectName, Project.Configuration.OutputType type)
        {
            // Declares the target for which we build the project. This time we add
            // the additional OutputType fragment, which is a prebuilt fragment
            // that help us specify the kind of library output that we want.
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2022,
                Optimization.Debug | Optimization.Release));

            Name = projectName;
            ProjectType = type;
        }

        [Configure]
        public virtual void ConfigureAll(Project.Configuration conf, Target target)
        {
            // This is the name of the configuration. By default, it is set to
            // [target.Optimization] (so Debug or Release), but both the debug and
            // release configurations have both a shared and a static version so
            // that would not create unique configuration names.
            //conf.Name = ProjectName;
            conf.Name = @"[target.Optimization][target.OutputType]";

            // Gives a unique path for the project because Visual Studio does not
            // like shared intermediate directories.
            conf.ProjectPath = Path.Combine(Defines.Paths.INFERNO_PROJECTS, Name);
            conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);
            conf.Output = ProjectType;
        }
    }
}