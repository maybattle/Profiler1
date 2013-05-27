using System;
using System.Collections.Generic;

using System.Text;


namespace SampleToProfile
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("BeamUp me up, scotty!");
            var beamer = new Beamer();
            beamer.Start("Beamer is active.");
            Console.WriteLine("Press Enter to stop beaming.");
            var state = beamer.GetBeamerState();
            Console.WriteLine(state);
            beamer.SetNumberOfHumansToBeam(1);
            beamer.BeamUp("James T. Kirk");
            Console.ReadLine();
        }
    }


    public class Beamer{
        private int _numberOfHumans;

        public void Start(string message)
        {
            Console.WriteLine(message);
        }

        public string GetBeamerState(){
            return "Beamer is OK.";
        }

        public object BeamUp(object objectToBeam){
            return objectToBeam;
        }

        public void SetNumberOfHumansToBeam(object numberOfHumans){
            _numberOfHumans = Convert.ToInt32(numberOfHumans);
        }
    }
}
