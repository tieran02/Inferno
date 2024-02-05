using System;
using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

namespace Inferno
{
    [Generate]
    public class EditorProject : InfernoApp
    {
        public EditorProject() : base("editor")
        {
            SourceRootPath = Defines.Paths.INFERNO_EDITOR_SRC;
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.AddPrivateDependency<CoreLib>(target);
            conf.AddPrivateDependency<GraphicsLib>(target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_CORE);
            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS);

            string dest = Path.Combine(Defines.Paths.INFERNO_PROJECTS, Name, "data/");
            CopyDirectory(Defines.Paths.INFERNO_DATA, dest, true);
        }

        void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
        {
            // Get information about the source directory
            var dir = new DirectoryInfo(sourceDir);

            // Check if the source directory exists
            if (!dir.Exists)
                throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

            // Cache directories before we start copying
            DirectoryInfo[] dirs = dir.GetDirectories();

            // Create the destination directory
            Directory.CreateDirectory(destinationDir);

            // Get the files in the source directory and copy to the destination directory
            foreach (FileInfo file in dir.GetFiles())
            {
                string targetFilePath = Path.Combine(destinationDir, file.Name);
                file.CopyTo(targetFilePath);
            }

            // If recursive and copying subdirectories, recursively call this method
            if (recursive)
            {
                foreach (DirectoryInfo subDir in dirs)
                {
                    string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
                    CopyDirectory(subDir.FullName, newDestinationDir, true);
                }
            }
        }
    }

}