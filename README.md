p1
==

B228's awesome projekt!

Komprimering af en kort besked



**To push:**

	git add . 	(Tilføjer alle nye dokumenter, men ikke dokumenter der er ændret i)

eller	

	git add file1.tex file2.tex

Commit (stadig på egen computer)

	git commit -m 'Kommentar her'

Push to github

	git push origin master


**To Pull:**

	git pull	


Texmaker hints:
==
Dansk ordbog:
--

http://www.stavekontrolden.dk/main/sspinputfiles/da_DK.dic
og
http://www.stavekontrolden.dk/main/sspinputfiles/da_DK.aff

Gemmes til samme mappe, og .dic filen vælges i "Options" > "Configure Texmaker" > "Editor", i feltet "Spelling dictionary"


Indsæt billede:
--
Eksempel:

	\includegraphics[scale=0.35]{Billeder/Huffman_tree_2.png}

mere info:
http://en.wikibooks.org/wiki/LaTeX/Importing_Graphics


Will become a heading
==============

Will become a sub heading
--------------

*This will be Italic*

**This will be Bold**

- This will be a list item
- This will be a list item

	Add a indent and this will end up as code

