<h1><p align = center> Welcome to IceDict </p></h1>
<h4><p align = center> The Inflectional Icelandic <-> English Dictionary </h4>

## What Can It Do?
IceDict is a Icelandic <-> English dictionary that supports:

1. Finding English definitions for Modern Icelandic words
2. Searching text in the said Modern Icelandic dictionary
3. Finding English definitions for Old Icelandic words in two Old Icelandic dictionaries (Zoëga, Cleasby & Vifgusson)
4. Searching text in the said Old Icelandic dictionaries
5. Querying inflections reversely: enter an inflected word of Modern Icelandic for a summary of all words that have this inflected form with the exact inflection information
6. Querying all inflectional forms of a single word. You may then choose which specific forms you wish to query.

The features above can all be found in the `Dictionaries` menu in the exact same order, each corresponding to a keyboard shortcut: CTRL + Number (on Windows) and CMD + Number (on macOS).

<br>

## How Do I Use It?
Once the application is ready, you will be greeted with a welcome page. The search panel and options panel are not yet clickable. Once you choose a dictionary, either from the menubar or by a keyboard shortcut, they will be available for use. It is same procedure with every new tab you create. 

<br>

## How Much Resource Does It Use?
This dictionary is designed for speed, so it might take up quite a bit of RAM. You may have already noticed it if you have resource manager on your OS opened up by now. An average RAM usage is approximately 750MB. You may have also noticed that it takes some time to load, and that is because to query a database with 550MB of data it needs to index all the entries. Fortunately this only happens at startup. 

<br>

## What Does It Look Like?
Since this dictionary is meant to be cross-platform, it should have the same layout and functionalities on all platforms but with a native look. The demonstration below are taken on macOS. For screenshots on other platforms, feel free to visit [Screenshots on Windows](https://github.com/scottsanett/IceDict_QtWidgets/issues/3). Unfortunately this application has not been adapted to support HiDPI scaling on Windows.

#### Welcome Screen
![Welcome Screen](images/screenshot1.jpg)

#### Right Click
Before delving into all the dictionaries, it is worth noting that one might select a word in the result panel and perform different kinds of searches by a right click. The action will create a new tab and perform the search there.
![right click search](images/screenshot14.jpg)

#### 1. Modern Icelandic -> English (online)
Since this dictionary requires internet access, the speed of the query is highly dependent on the speed of your internet access and your location. If your query yields multiple results or redirection, the alternatives are listed in the option panel.
![searching vera](images/screenshot2.jpg)

#### 2. Textual Search in Modern Icelandic Dictionary (online)
This uses the same dictionary as the one above, only that now it searches text indiscriminately in the dictionary.
![searching tired](images/screenshot3.jpg)

#### 3. Old Icelandic -> English
This feature searches words in the two Old Icelandic dictionaries available: *Zoëga* and *Cleasby & Vifgusson*. Since *Cleasby* provides some of the entries in all caps, the search is performed in an case-insensitive manner. The panel under the search panel provides all the options that are available in the dictionaries, and it changes as you type to serve as a spelling clue. The penal under that are the actual results that have been found in the dictionaries. 

![searching vera in Zoëga](images/screenshot4.jpg)
<p align=center><sup>search the word <b>vera</b> yields 4 alternatives</sup></p>
![searching vera in Cleasby](images/screenshot5.jpg)

#### 4. Textual Search in Old Icelandic Dictionaries
This feature works like #2 and #3 combined. All entries that have been found to match the search will be listed in the options panel. The word in search is marked red in the result.
![searching the word viking](images/screenshot6.jpg)
![searching the word viking](images/screenshot7.jpg)

#### 5. Search Inflections (Reversely)
This feature searches for all words and inflectional tags that match the word in query. 

![searching loga](images/screenshot8.jpg)
<p align=center><sup>search the word <b>loga</b> returns a neuter noun <b>log</b>, the genitive plural indefinite of which is <b>loga</b> etc.</sup></p>

#### 6. List All Forms
This feature provides functionalities to list all inflectional forms of a word that can be filtered as one likes. Since not all words of a particular part of speech have all the forms available for that part of speech (some verbs, for instance, don't have past participles), only the ones available for the word in query will be listed in the filter panel. All the filters are unchecked by default. 

Note that all requirements (voice, mood, tense, etc) of an inflectional category (impersonal, infinitive, imperative, etc) must be met for filtering to work. For non-impersonal verb forms, for instance, at least one option (e.g. active, middle) for each requirement (e.g. voice) needs to be checked, otherwise the result is blank.

![list all forms of kona](images/screenshot9.jpg)
<p align=center><sup>list all inflectional forms of the word <b>kona</b></sup></p>

![list only definite forms of kona](images/screenshot10.jpg)
<p align=center><sup>filter out all indefinite forms</sup></p>

![list only definite and nominative forms of kona](images/screenshot11.jpg)
<p align=center><sup>filter out all indefinite forms and all cases that are not nominative</sup></p>

![the verb vera](images/screenshot12.jpg)
<p align=center><sup>The verb <b>vera</b> doesn't have past participles</sup></p>

![the verb heyra](images/screenshot13.jpg)
<p align=center><sup>The verb <b>heyra</b> does</sup></p>


