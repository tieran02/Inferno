using System;
using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using Inferno;

namespace Inferno.Samples
{    
    public class SampleProject : InfernoApp
    {
        static public readonly string INFERNO_SAMPLES = Path.Combine(Defines.Paths.INFERNO_ROOT, @"samples\");
        static public void AddSampleProjects(Solution.Configuration conf, Target target)
        {
            conf.AddProject<SampleHelloTriangle>(target);
        }

        public SampleProject(string projectName) : base(projectName)
        {
            SourceRootPath = $"{INFERNO_SAMPLES}/{projectName}";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "Samples";

            conf.AddPrivateDependency<CoreLib>(target);
            conf.AddPrivateDependency<GraphicsLib>(target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_CORE);
            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS);

            conf.PrecompHeader = "";
            conf.PrecompSource = "";

            conf.ProjectPath = Path.Combine($"{Defines.Paths.INFERNO_PROJECTS}/samples", Name);
        }
    }

    [Generate]
    public class SampleHelloTriangle : SampleProject
    {
        readonly string SampleRootDir;
        public SampleHelloTriangle() : base("hellotriangle")
        {
            SampleRootDir = $"{INFERNO_SAMPLES}/{Name}";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            string inDataFolder = $"{SampleRootDir}/data/shaders";
            string outDataFolder = $"{conf.ProjectPath}/data/shaders";
            ShaderUtil.CompileShadersPreBuild(conf,inDataFolder,outDataFolder);
        }
    }

}