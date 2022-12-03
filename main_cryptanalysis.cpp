#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;

typedef array<pair<char, double>, 26> FreqArray;

class VigenereCryptanalysis
{
private:
  array<double, 26> targets;
  array<double, 26> sortedTargets;

  // TO COMPLETE

public:
  VigenereCryptanalysis(const array<double, 26> &targetFreqs)
  {
    targets = targetFreqs;
    sortedTargets = targets;
    sort(sortedTargets.begin(), sortedTargets.end());
  }

  pair<string, string> analyze(string input)
  {
    // En majuscules
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);

    string key = "Je ne sais pas encore";
    string result = "Ca vient, pour l'instant je calcule l'IC";

    map<uint, double> ICParTailleDeClef = calculICParTailleDeClef(input, 2, 15);
    std::vector<uint> tailleClefsATester = clefsICEleve(ICParTailleDeClef);

    vector<std::string> clefs;
    vector<std::string> messages;

    for (uint tailleClef : tailleClefsATester)
    {
      key = trouverClef(tailleClef, input);
      clefs.push_back(key);
      messages.push_back(dechiffrerVigenere(input, key));
    }
    if (clefs.size() > 0 && messages.size() > 0)
      return make_pair(messages[0], clefs[0]);
    else
      return make_pair("", "");
  }

private:
  std::string trouverClef(uint tailleClef, std::string texteChiffre)
  {
    string clef = "";
    for (uint lettreClef = 0; lettreClef < tailleClef; ++lettreClef)
    {
      // On prend les lettres codées par la même lettre de la clef
      std::string sequence;
      for (uint i = lettreClef; i < texteChiffre.size(); i += tailleClef)
      {
        sequence.append(texteChiffre.substr(i, 1));
      }
      clef += decrypterCesar(sequence);
    }
    return clef;
  }

  char decrypterCesar(std::string sequence)
  {
    std::vector<std::pair<uint, double>> vecteurChi; // Utile uniquement pour debug
    std::pair<uint, double> meilleurChi;
    bool nonInitialise = true;

    for (uint clefCesar = 0; clefCesar < 26; ++clefCesar)
    {
      // Déchiffrement de César
      std::string cesarDechiffre = dechiffreCesar(sequence, clefCesar);

      // Statistiques de la répartition des lettre
      double chi = calculChiRacine(cesarDechiffre);

      if (nonInitialise)
      {
        meilleurChi = std::pair<uint, double>{clefCesar, chi};
        nonInitialise = false;
      }
      else if (chi < meilleurChi.second)
        meilleurChi = std::pair<uint, double>{clefCesar, chi};
      vecteurChi.push_back(std::pair<uint, double>{clefCesar, chi});
    }
    return 'A' + meilleurChi.first;
  }

  string dechiffrerVigenere(string text, string key)
  {
    string out = text;

    // ADD THE VIGENERE DECRYPTION
    for (uint i = 0; i < text.size(); ++i)
    {
      char c = (out[i] - 'A');
      char k = (key[i % key.size()] - 'A');
      out[i] = 'A' + ((c - k + 26) % 26);
    }

    return out;
  }

  std::string dechiffreCesar(std::string chiffre, uint clefCesar)
  {
    std::string cesarDechiffre = chiffre;
    for (uint i = 0; i < chiffre.size(); ++i)
    {
      cesarDechiffre[i] = (((cesarDechiffre[i] - 'A') + clefCesar) % 26) + 'A';
    }
    return cesarDechiffre;
  }

  double calculChiRacine(std::string str)
  {
    double chi = 0;
    for (char c = 'A'; c <= 'Z'; ++c)
    {
      double cq = totalLettreDansString(c, str);
      double eq = targets[c - 'A'] * str.size();

      chi += ((cq - eq) * (cq - eq)) / eq;
    }
    return chi;
  }

  std::vector<uint> clefsICEleve(map<uint, double> ICParTailleDeClef, double tolerance = 0.01)
  {
    std::map<uint, double>::iterator maxIC = std::max_element(ICParTailleDeClef.begin(), ICParTailleDeClef.end(), [](const std::pair<uint, double> &a, const std::pair<uint, double> &b)
                                                              { return a.second < b.second; });
    std::vector<uint> clefs;
    std::for_each(ICParTailleDeClef.begin(), ICParTailleDeClef.end(), [&clefs, maxIC, tolerance](const std::pair<uint, double> &a)
                  {
      if(a.second > maxIC->second - tolerance)
        clefs.push_back(a.first); });
    return clefs;
  }

  std::map<uint, double> calculICParTailleDeClef(string input, uint tailleClefMin, uint tailleCLefMax)
  {
    map<uint, double> ICParTailleDeClef;

    for (uint keySize = tailleClefMin; keySize < tailleCLefMax; ++keySize)
    {
      double ICTotal = 0;
      // Pour chaque taille de clef possible
      for (uint i = 0; i < keySize; ++i)
      {
        // Pour chaque caractère de la clef
        std::string lettres;
        for (uint j = i; j < input.size(); j += keySize)
        {
          // On prend les lettres codées par la même lettre de la clef
          lettres.append(input.substr(j, 1));
        }
        ICTotal += calculIC(lettres, lettres.size());
      }
      ICParTailleDeClef.insert(std::pair<uint, double>(keySize, ICTotal / keySize));
    }
    return ICParTailleDeClef;
  }

  uint totalLettreDansString(char c, std::string str)
  {
    double total = 0; // nombre de fois où cette lettre apparait

    for (uint i = 0; i < str.size(); ++i)
      if (str[i] == c)
        total++;
    return total;
  }

  double calculIC(std::string lettres, uint n)
  {
    double IC = 0;
    for (char c = 'A'; c <= 'Z'; ++c)
    {
      double nq = totalLettreDansString(c, lettres); // nombre de fois où cette lettre apparait

      double probaLettre = (nq * (nq - 1)) / (n * (n - 1));
      IC += probaLettre;
    }
    return IC;
  }
};

int main()
{
  string input = "zbpuevpuqsdlzgllksousvpasfpddggaqwptdgptzweemqzrdjtddefekeferdprrcyndgluaowcnbptzzzrbvpssfpashpncotemhaeqrferdlrlwwertlussfikgoeuswotfdgqsyasrlnrzppdhtticfrciwurhcezrpmhtpuwiyenamrdbzyzwelzucamrptzqseqcfgdrfrhrpatsepzgfnaffisbpvblisrplzgnemswaqoxpdseehbeeksdptdttqsdddgxurwnidbdddplncsd";

  array<double, 26> english = {
      0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228,
      0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025,
      0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987,
      0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
      0.01974, 0.00074};

  array<double, 26> french = {
      0.0811, 0.0081, 0.0338, 0.0428, 0.1769, 0.0113,
      0.0119, 0.0074, 0.0724, 0.0018, 0.0002, 0.0599,
      0.0229, 0.0768, 0.0520, 0.0292, 0.0083, 0.0643,
      0.0887, 0.0744, 0.0523, 0.0128, 0.0006, 0.0053,
      0.0026, 0.0012};

  VigenereCryptanalysis vc_en(english);
  pair<string, string> output_en = vc_en.analyze(input);

  cout << "Key: " << output_en.second << endl;
  cout << "Text: " << output_en.first << endl;

  VigenereCryptanalysis vc_fr(french);
  pair<string, string> output_fr = vc_fr.analyze(input);

  cout << "Key: " << output_fr.second << endl;
  cout << "Text: " << output_fr.first << endl;
}
