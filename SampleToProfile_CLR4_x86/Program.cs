using System;
using System.Collections.Generic;

using System.Text;


namespace SampleToProfile
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Beam me up, scotty!");
            var beamer = new Beamer();
            beamer.Start("Beamer is active.");
            Console.WriteLine("Press Enter to stop beaming.");
            var state = beamer.GetBeamerState();
            Console.WriteLine(state);
            beamer.Beam("James T. Kirk");
            Console.ReadLine();
        }
    }


    public class Beamer
    {
        public void Start(string message)
        {
            Console.WriteLine(message);
        }

        public string GetBeamerState(){
            return "Beamer is OK.";
        }

        public object Beam(object objectToBeam){
            return objectToBeam;
        }
    }
}
