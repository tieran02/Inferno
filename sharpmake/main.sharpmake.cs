using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using Inferno;
using Inferno.Samples;

[module: Sharpmake.Include("engineDefs.sharpmake.cs")]
[module: Sharpmake.Include("core.sharpmake.cs")]
[module: Sharpmake.Include("graphics.sharpmake.cs")]
[module: Sharpmake.Include("thirdparty.sharpmake.cs")]
[module: Sharpmake.Include("shaders.sharpmake.cs")]
[module: Sharpmake.Include("samples.sharpmake.cs")]

namespace Inferno
{

// Represents the solution that will be generated and that will contain the
// project with the sample code.
[Generate]
public class InfernoSolution : Solution
{
    public InfernoSolution()
    {
        // The name of the solution.
        Name = "Inferno";

        // As with the project, define which target this solution builds for.
        // It's usually the same thing.
        AddTargets(new Target(
            Platform.win64,
            DevEnv.vs2022,
            Optimization.Debug | Optimization.Release));
    }

    // Configure for all 4 generated targets. Note that the type of the
    // configuration object is of type Solution.Configuration this time.
    // (Instead of Project.Configuration.)
    [Configure]
    public void ConfigureAll(Solution.Configuration conf, Target target)
    {
        // Puts the generated solution in the /generated folder too.
        conf.SolutionPath = Defines.Paths.INFERNO_PROJECTS;;

        //Only have sample project now, till an editor is implemented
        SampleProject.AddSampleProjects(conf, target);
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Sharpmake.Arguments arguments)
    {
        // Tells Sharpmake to generate the solution described by
        // BasicsSolution.
        arguments.Generate<InfernoSolution>();
    }
}

}