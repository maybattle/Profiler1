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
            new Beamer().Start("Beamer is active.");
            Console.WriteLine("Press Enter to stop beaming.");
            Console.ReadLine();
        }
    }


    public class Beamer
    {
        public void Start(string message)
        {
            Console.WriteLine(message);
        }
    }
}
