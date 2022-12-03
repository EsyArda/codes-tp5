# Théorie des Codes, TP5 - Cryptographie Classique : Chiffrement de Vigenère

> Lilian Hiault

Les parties chiffrement, déchiffrement de Vigenère et le calcul de l'IC ont été faites conjoitement avec Marian Poirot et Dylan Rodrigues. Le reste du TP a été terminé seul.

- [Théorie des Codes, TP5 - Cryptographie Classique : Chiffrement de Vigenère](#théorie-des-codes-tp5---cryptographie-classique--chiffrement-de-vigenère)
  - [Chiffrement de Vigenère](#chiffrement-de-vigenère)
  - [Déchiffrement du code de Vigenère](#déchiffrement-du-code-de-vigenère)
  - [Décryptage du code de Vigenère](#décryptage-du-code-de-vigenère)
    - [Identifier la taille de la clé](#identifier-la-taille-de-la-clé)
    - [Trouver la valeur de la clé](#trouver-la-valeur-de-la-clé)


## Chiffrement de Vigenère

Pour chiffrer avec Vigenère, on décale chaque caractère du texte en clair par la caractère correspondant de la clé en la répétant si le texte est plus long.

```c++
for (uint i = 0; i < text.size(); ++i)
{
    resultat[i] = 'A' + (((entree[i] - 'A') + (key[i % key.size()] - 'A')) % 26);
}
```

## Déchiffrement du code de Vigenère

On utilise la même techenique mais on soustrait la clé au lieu de l'ajouter. Ici on fait attention à avoir une valeur positive avant le modulo car il ne gère pas correctement les nombres négatifs.

```c++
for (uint i = 0; i < text.size(); ++i)
{
    char lettre = (chiffre[i] - 'A');
    char cle = (key[i % key.size()] - 'A');
    dechiffre[i] = 'A' + ((lettre - cle + 26) % 26);
}
```

## Décryptage du code de Vigenère

### Identifier la taille de la clé

Pour chaque taille de clé possible entre 2 bornes définies, on crée la séquence de lettres qui sont codées par le même caractère de la clé. Par exemple si la clé est de taille 3, on prend une lettre sur 3.
On calcule l'indice de coincidence IC pour savoir si cette séquence de lettres pourrait être française ou anglaise.

```c++
for (uint keySize = tailleClefMin; keySize < tailleCLefMax; ++keySize)
{
    // Pour chaque taille de clé possible
    double ICTotal = 0;
    for (uint i = 0; i < keySize; ++i)
    {
        // Pour chaque caractère de la clé
        std::string lettres;
        for (uint j = i; j < input.size(); j += keySize)
        {
            // On prend les lettres codées par la même lettre de la clé
            // pour les concaténer dans une séquence
            lettres.append(input.substr(j, 1));
        }
        ICTotal += calculIC(lettres, lettres.size());
    }
}
```

### Trouver la valeur de la clé

Lorsqu'on a obtenu la taille de la clé, on découpe le texte chiffré en séquences qui sont codées par le même caractère de la clé.
Puis, on attaque le code César en testant les 26 décalages et en validant ceux qui dont les probabilités d'apparition des lettres se rapprochent le plus des valeurs qu'on connaît dans la langue.
Dès qu'on a obtenu le décalage du code César, on ajoute la lettre correspondant à la clé finale de chiffrement de Vigenère.

```c++
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
```

Pour attaquer le code de César, on prend les séquences de lettres qu'on déchuffre avec les 26 décalages possibles.
On calcule la valeur de χ² grâce aux statistiques de présences dans la langue et daans le texte d'un même caractère.
On garde le chiffrement de César dont la valeur de χ² est la plus faible.

```c++
for (uint clefCesar = 0; clefCesar < 26; ++clefCesar)
{
    // Déchiffrement de César
    std::string cesarDechiffre = dechiffreCesar(sequence, clefCesar);

    // Statistiques de la répartition des lettre
    double chi = calculChiCarre(cesarDechiffre);

    if (nonInitialise)
    {
        meilleurChi = std::pair<uint, double>{clefCesar, chi};
        nonInitialise = false;
    }
    else if (chi < meilleurChi.second)
        meilleurChi = std::pair<uint, double>{clefCesar, chi};
}
```

Lorsqu'on a obtenu la clé, on peut déchiffrer le message reçu.
Pour l'instant, le calcul de la taille de clé fonctionne comme prévu mais le calcul de χ² ne permet pas de trouver la vraie clé.