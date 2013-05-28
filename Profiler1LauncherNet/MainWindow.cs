using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Profiler1LauncherNet
{
    public partial class MainWindow : Form
    {
        
        public MainWindow()
        {
            InitializeComponent();
            MessageLabel.Text = @"ACHTUNG:"
            +Environment.NewLine+@"Der Start des zu profilenden Programms wird "
            + Environment.NewLine + @"aufgrund des Profilers um einen Faktor größer 10 langsamer ausgeführt."
            + Environment.NewLine + @"Auch das Programm selbst läuft deutlich langsamer."
            + Environment.NewLine + @"Besonders beim Profilen der EntityFramework Version ist etwas Geduld erforderlich."
            + Environment.NewLine + @"Die Laufzeit kann verbessert werden, "
            + Environment.NewLine + @"wenn die Performance des Profilers verbessert wird.";
            SelectedProcessToProfileTextbox.Text = Path.Combine(Directory.GetCurrentDirectory(), @"SampleToAnalyzeEF_Linq2Sql.exe");
        }

        private void SelectApplicationButton_Click(object sender, EventArgs e)
        {
            if (SelectProcessToProfileFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                SelectedProcessToProfileTextbox.Text = SelectProcessToProfileFileDialog.FileName;
            }
        }

        private void StartProfilingButton_Click(object sender, EventArgs e)
        {
            try{
                Cursor.Current = Cursors.WaitCursor;
                var startInfo = new ProcessStartInfo(SelectedProcessToProfileTextbox.Text);
                //Profiling generell aktivieren
                startInfo.EnvironmentVariables.Add("COR_ENABLE_PROFILING","1");
                //obligatorisch, da die CLASSID immer benoetigt wird
                startInfo.EnvironmentVariables.Add("COR_PROFILER", "{DFE1C658-1DB9-49A4-9BE4-D08E9A65653D}");
                //Ab CLR 4 zusaetzlich optional moeglich, um die Registrierung zu vermeiden.
                //vollstaendiger Pfad zur Profiler DLL, die von der CLR aktiviert werden soll.
                startInfo.EnvironmentVariables.Add("COR_PROFILER_PATH", @"Profiler1.dll");

                //Kompatibilitaetsmodus aktivieren. Es koennen CLR4 Anwendungen profiled werden. 
                //Neue Funktionen werden nicht verwendet.
                startInfo.EnvironmentVariables.Add("COMPLUS_ProfAPI_ProfilerCompatibilitySetting", "EnableV2Profiler");

                //http://msdn.microsoft.com/en-us/library/ee461607.aspx
                //Die zu profilende CLR Version wird vorgegeben
                startInfo.EnvironmentVariables.Add("CLR_VERSION","4");
                startInfo.UseShellExecute = false;
                var process = Process.Start(startInfo);
            }
            finally{
                Cursor.Current = Cursors.Default;
            }
        }
    }
}
