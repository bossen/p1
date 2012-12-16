using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace Counting
{
    class Program
    {
        static void Main(string[] args)
        {
            StreamWriter sw = new StreamWriter("result.txt",false, Encoding.Unicode);   
            string filename;
            string body;
            long total = 0;
            var entries = new Dictionary<char, int>();
            List<char> chars = new List<char>();
            List<int> amounts = new List<int>();
            do
            {
                Console.Write("Input File (\"-list\" to see files): ");
                filename = Console.ReadLine();
                if (filename == "-list")
                {
                    string[] dir = Directory.GetFiles(AppDomain.CurrentDomain.BaseDirectory);
                    foreach (string n in dir)
                        Console.WriteLine(Path.GetFileName(n.ToString()));
                }
            } while (filename == "-list");

            if (File.Exists(filename) && filename != "result.txt")
            {
                StreamReader sr = new StreamReader(filename, Encoding.UTF8);
                StreamReader sra = new StreamReader(filename, Encoding.Default);
                if (Path.GetExtension(filename).Contains("xml"))
                {
                    Console.WriteLine("Treating as SMS XML Document");
                    XmlReaderSettings xrs = new XmlReaderSettings();
                    xrs.CheckCharacters = false;
                    XmlReader reader = XmlReader.Create(sra, xrs);

                    while (reader.Read())
                    {
                        if (reader.Name == "sms" && reader.NodeType == XmlNodeType.Element)
                        {
                            body = reader.GetAttribute("body").ToString();
                            foreach (char c in body)
                            {
                                if (chars.IndexOf(c) == -1)
                                {
                                    chars.Add(c);
                                    amounts.Add(1);
                                }
                                else
                                {
                                    amounts[chars.IndexOf(c)]++;
                                }
                            }
                        }
                    }
                    foreach (char c in chars)
                    {
                        entries.Add(c, amounts[chars.IndexOf(c)]);
                    }
                    foreach (var item in entries.OrderBy(i => -i.Value))
                    {
                        sw.WriteLine("{0}:{1}", item.Key, item.Value);
                        total += item.Value;
                    }
                }
                else
                {
                    Console.WriteLine("Treating as Unicode text document");
                    string line;
                    while ((line = sr.ReadLine()) != null)
                    {

                        foreach (char c in line)
                        {
                            if (chars.IndexOf(c) == -1)
                            {
                                chars.Add(c);
                                amounts.Add(1);
                            }
                            else
                            {
                                amounts[chars.IndexOf(c)]++;
                            }
                        }
                    }
                    foreach (char c in chars)
                    {
                        entries.Add(c, amounts[chars.IndexOf(c)]);
                    }

                    foreach (var item in entries.OrderBy(i => -i.Value))
                    {
                        sw.WriteLine("{0}:{1}", item.Key, item.Value);
                        total += item.Value;
                    }
                }
                Console.WriteLine("Success! {0} Characters read.\nExported to file: result.txt", total);
                sra.Close();
                sr.Close();
            }
            else
            {
                Console.WriteLine("File does not exist or is not a valid choice");
            }
            sw.Close();
            Console.Read();
            

        }
    }
}