#include "../types.h"
#include "miniwin_sdl.h"
#include "stubs.h"


int SCREEN_WIDTH = 800; // 1024×768
int SCREEN_HEIGHT = 600;

int Window_Width = 800;
int Window_Height = 600;

int LogoWidth;
int LogoHeight;

SDL_Rect textureRect;
SDL_Rect windowRect;
SDL_Rect CusorLocation;

SDL_Rect SinglePlrBox;
SDL_Rect MultiPlrBox;
SDL_Rect ReplayIntroBox;
SDL_Rect ShowCreditsBox;
SDL_Rect ExitBox;

// DiabloMenu Items

SDL_Rect MainMenuItemsWRect;
SDL_Rect MainMenuItemsTRect;
SDL_Rect CreateHeroBox;
SDL_Rect SinglePlayerMenuCancelBox;

SDL_Event input;

bool SinglePlayerMenuItemsLoaded = 0;
bool DiabloImageLoaded = 0;
bool DiabloMainMenuListLoaded = 0;

_uiheroinfo heroarray[10];

struct timespec ts;

//////////////////////////////////////
// things I need to unload...

int FontLoaded = 0;

SDL_Texture *CreditsTexture;
SDL_Texture *MainMenuItemsTexture;
SDL_Texture *DiablologoAnimT;
SDL_Texture *CursorTexture;
SDL_Texture *MenuSelectNewHeroTexture;
SDL_Texture *CreateHeroDialogTextureW;
SDL_Texture *CreateHeroDialogTextureR;
SDL_Texture *CreateHeroDialogTextureS;

SDL_Surface *CreditsSurface;
SDL_Surface *MainMenuItemsSurface;
SDL_Surface *MenuSelectNewHeroSurface;
SDL_Surface *CreateHeroDialogSurface;
SDL_Surface *CursorImg;
SDL_Surface *DiabloTitle;

int bloaded = 0;
void *pPcxCreditsImage;
int gdwCreditsWidth;
int gdwCreditsHeight;

////////////////
// new vars

int gb_Lfont_str_len;
int gb_Lfont_pix_width;
int the_pcx_frame;

char *the_long_credits[] = {"",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "$-- Devilution --",
                            "Reversed by Andi \"GalaXyHaXz\"",
                            "",
                            "$-- Meet the Team --",
                            "seritools",
                            "mewmew",
                            "nomdenom",
                            "AJenbo",
                            "ApertureSecurity",
                            "",
                            "$-- Special Thanks --",
                            "The Sanctuary preservation project",
                            "Jarulf's guide to the Devil himself",
                            "",
                            "",
                            "$...The Devil is in the details",
                            "",
                            "",
                            "$Game Design",
                            "Blizzard North",
                            "",
                            "$Senior Designers",
                            "Erich Schaefer, David Brevik",
                            "",
                            "$Additional Design",
                            "Max Schaefer, Eric Sexton, Kenneth Williams",
                            "",
                            "$Lead Programmer",
                            "David Brevik",
                            "",
                            "$Senior Programmers",
                            "Richard Seis, Doron Gartner,",
                            "Patrick Wyatt, Mike O'Brien",
                            "",
                            "$Programming",
                            "Peter Brevik, Robin van der Wel, Jon Morin",
                            "",
                            "$Special Guest Programmers",
                            "Collin Murray, Frank Pearce",
                            "",
                            "$Battle.net Programming",
                            "Mike O'Brien, Mike Morhaime, Dan Liebgold",
                            "",
                            "$Serial Communications Programming",
                            "Jeff Strain, Mike O'Brien",
                            "",
                            "$Installer Programming",
                            "Bob Fitch, Patrick Wyatt",
                            "",
                            "$Art Directors",
                            "Erich Schaefer, Samwise Didier",
                            "",
                            "$Artwork",
                            "Michio Okamura, Eric Sexton, Ben Haas, Patrick Tougas,",
                            "Kelly Johnson, Erich Schaefer, Max Schaefer, Hugh Tom",
                            "Byrne, Roman Kenney, Samwise Didier, Dave Berggren,",
                            "Micky Neilson, Eric Flannum, Justin Thavirat, Brian",
                            "Sousa, Stu Rose, Nick Carpenter, Joeyray Hall",
                            "",
                            "$Technical Artwork",
                            "Christopher Root, Mark Sutherland,",
                            "Robert Djordjevich, Jeffrey Vaughn",
                            "",
                            "$Cinematic Art Directors",
                            "Duane Stinnett, Matt Samia",
                            "",
                            "$3D Cinematic Artwork",
                            "Duane Stinnett, Matt Samia, Rob McNaughton,",
                            "Eric Flannum, Dave Pursley, Micky Neilson,",
                            "Maxx Marshall, Trevor Jacobs, Brian Sousa,",
                            "Samwise Didier, Ron Smorynski, Ben Haas, Patrick Tougas",
                            "",
                            "$Cinematic Technical Artwork",
                            "Joeyray Hall ",
                            "",
                            "$Executive Producer",
                            "Allen Adham",
                            "",
                            "$Producer",
                            "Bill Roper",
                            "",
                            "$Associate Producer",
                            "James Phinney",
                            "",
                            "$Diablo Strike Team",
                            "Allen Adham, Ron Millar, Mike O'Brien,",
                            "James Phinney, Bill Roper, Patrick Wyatt",
                            "",
                            "$Music",
                            "Matt Uelmen",
                            "",
                            "$Sound Design",
                            "Matt Uelmen, Glenn Stafford",
                            "",
                            "$Cinematic Music & Sound",
                            "Glenn Stafford, Matt Uelmen",
                            "",
                            "$Voice Production, Direction & Casting",
                            "Mark Schwarz, Glenn Stafford, Bill Roper",
                            "",
                            "$Script & Story",
                            "Erich Schaefer, Eric Sexton,",
                            "Chris Metzen, Bill Roper",
                            "",
                            "$Voice Editing",
                            "Glenn Stafford",
                            "",
                            "$Voices",
                            "Michael Haley, Glynnis Talken, Michael Gough,",
                            "Paul Eiding, Lani Minella, Steve Brodie, Bill Roper,",
                            "Mark Schwarz, Glenn Stafford, Chris Metzen, Max Schaefer",
                            "",
                            "$Recording Engineer",
                            "Robert Koenigsberg",
                            "",
                            "$Manual Design & Layout",
                            "Peter Underwood, Jeffrey Vaughn,",
                            "Chris Metzen, Bill Roper",
                            "",
                            "$Manual Artwork",
                            "Samwise Didier, Chris Metzen,",
                            "Roman Kenney, Maxx Marshall",
                            "",
                            "$Provisional Director of QA (Lead Tester)",
                            "Shane Dabiri",
                            "",
                            "$QA Assault Team (Testers)",
                            "Christopher Cross, Mike Givens, Dave Hale,",
                            "Brian \"Doc\" Love, Hung Nguyen, Dean Shipley,",
                            "Chris Sigaty, Ian Welke, Jamie Wiggs",
                            "",
                            "$QA Special Ops Team (Compatibility Testers)",
                            "Adam Maxwell, Russell \"Rug\" Miller",
                            "",
                            "$QA Artillery Support (Additional Testers) ",
                            "Rich Alcazar, Mike Jones, Rev. Dan Moore, Matt Morris,",
                            "Mark Pfeiffer, Harish \"Pepe the Enforcer\" Rao, Chris Millar",
                            "",
                            "$QA Counterintelligence",
                            "Alan Dabiri, Jesse McReynolds, Walter Takata",
                            "",
                            "$Order of Network Information Services",
                            "Robert Bridenbecker, Jason \"Brother Shaggy\" Schmit,",
                            "Jamie \"Brother Gug\" Steigerwald, Richard Seis ",
                            "",
                            "$Customer Support",
                            "John Schwartz, Vic Larson, Chad Chaudry, Mick Yanko,",
                            "Brandon Riseling, Kirk Mahony, Tony Hardy, Richard",
                            "Amtower, Collin Smith, Andrew Edwards, Roger Eberhart",
                            "",
                            "$Sales",
                            "Jorge Rivero, Rob Beatie",
                            "",
                            "$Dunsel",
                            "Alan Dabiri",
                            "",
                            "$Mr. Dabiri's Background Vocalists",
                            "Karin Colenzo, Christina Cade,",
                            "Kim Farrington, Melissa Edwards",
                            "",
                            "$Public Relations",
                            "Linda Duttenhaver, Susan Wooley",
                            "",
                            "$Marketing",
                            "John Patrick, Steve Huot, Neal Hubbard, Kathy Carter",
                            "",
                            "$International Sales",
                            "Ralph Becker, Chris Yoshimura",
                            "",
                            "$U.S. Sales",
                            "Todd Coyle, Danny Kearns",
                            "",
                            "$Manufacturing",
                            "John Goodman, Tom Bryan",
                            "",
                            "$Legal & Business",
                            "Paula Duffy, Eric Roeder, Paul Sams, Kenneth Williams",
                            "",
                            "$Special Thanks To",
                            "Primenet, Earthlink, AOL, Compuserve, ATI, Matrox, STB, ",
                            "Creative Labs, Logitech, U.S. Robotics, Gravis, Cyrix",
                            "",
                            "$Thanks To",
                            "Ian Colquhoun, Rod Shean, Brian Fitzgerald, Sharon Schafer, Todd ",
                            "Schafer, Richard and Sandra Schaefer, Rick Bowles, Greg Bogden, ",
                            "Cindy Sievert, Brad Mason, Chuck Goldman, Karin Colenzo, Wendy ",
                            "Brevik, Jennie Brevik, Hanna Carroll, Katie Newell, Karen Weiss, ",
                            "Kurt Beaver, Stan McDonald, Brian Sexton, Michael Wan, Megan ",
                            "Williams, Jessica Gensley, Beth Ann Parks, Brian Piltin, Johnathan ",
                            "Root, Brett Supernaw, Sabeer Bhatia, Mark Rearick, Brad Mason, ",
                            "Diane Kodama, Bernadette Sexton, Barbara Uhlmann, Patricia",
                            "McDonald, Kris Kelley, Alissa Vaage, Denise Hernandez, Paula ",
                            "White, Rachel Marie Hawley, Laura Gaber, Isaac Matarasso,",
                            "Susan Stafford, Gretchen Witte, Jason Sailor, Eufemia Palomino, ",
                            "Nathalie Didier, Nicole Welke, Dawn Caddel, Anna-Marcelle ",
                            "Okamura, Amber Okamura, Megan Okamura Tracey McNaughton, ",
                            "Trisha Ann Ochoa, Annie Strain, Dr. Gregory T. Street, Ray the Soda ",
                            "Guy, Sam Raimi, A. Guinness Brewing Co., John Woo, Chow Yun Fat, ",
                            "Jackie Chan,Proposition 215, Rumiko Takahashi, Bunchy & Mason, ",
                            "The Friends of Stan, The Zoo Crew, Team Hamro, Brazil 2001, RUSH, ",
                            "Cornholio, THE BROS., Dar, Emeril Lagasse, Tom Waits, Ice-Cube, ",
                            "The Strike Team Deflectors, Tony Levin, Big Jim and the Twins, ",
                            "Jag`rmeister, The Megasphere, Condor, The Allower, The Sunday ",
                            "Night Group, Gravitar, Steinway Pianos, Round Table Pizza, The Poxy ",
                            "Boggards, Urban Mystic & Co., Caffeine, Hunter Rose, Marly ",
                            "mediums in the box, sweet Poteen, Dave Scarpitti, TheByter, Bim ",
                            "Man, Krissann for color, Patty at Avalon Tattoo, James, Whaleboy, ",
                            "Dunsel Training Institute, monkeys, Dob Bole, Rootes Group, Porshe, ",
                            "Bongo, Scarlett, Apollo, The House of Mediocrity, Amelias, The King, ",
                            "Saag and all the folks at Davidson",
                            "",
                            "$In memory of",
                            "Lawrence and Barbara Williams",
                            "David A. Hargrave",
                            "Tony \"Xatre\" Collin",
                            "Thomas H Sexton",
                            "",
                            "$Very Special Thanks to",
                            "Bob and Jan Davidson",
                            "Walter Forbes",
                            "",
                            "$The Ring of One Thousand",
                            "Andrew Abernathy, Christopher Abramo, David Adams, David ",
                            "Adcox, Marko Agterberg, Cory Aiken, Judah Altaras, John ",
                            "Alvarez, Jose Alvarez, Richard Amable, Alexander Amaral, ",
                            "Scott Amis, Vincent Amoroso, Mark An, David Andersen, Jason ",
                            "Andrew Abernathy, Christopher Abramo, David Adams, David ",
                            "Adcox, Marko Agterberg, Cory Aiken, Judah Altaras, John ",
                            "Alvarez, Jose Alvarez, Richard Amable, Alexander Amaral, ",
                            "Scott Amis, Vincent Amoroso, Mark An, David Andersen, Jason ",
                            "Andersen, Aaron Anderson, George Anderson, Matthew ",
                            "Anderson, Michael Anderson, Sean Anderson, Robert Andrade, ",
                            "Cerny Andre, Michael Andreev, Devin Angle, Brian Apple, ",
                            "Brian Arbuthnot, Billy Arden, Dorian Arnold, Andre Arsenault, ",
                            "Erik Asplund, Mark Assam, John Athey, Jason Attard, Jeff ",
                            "Atwood, Ricky Au, Scott Avery, Rand Babcock, Steve Babel, ",
                            "Raymond Bacalso, Ed Bachta, Steven Back, Scott Baeder, Alex ",
                            "Baevski, Scott Bailey, Kenneth Baird, Thomas Baker, Todd ",
                            "Bala, Jan Ball, Greg Baltz, Blake Baltzley, Doug Bambrick, Wes ",
                            "Bangerter, Paul Barfuss, Chris Barghout, Dave Barnebey, Jon ",
                            "Barnhart, Terje Barth, Nicole Baskin, Bernard Baylen, Ryan ",
                            "Bear, Phil Bedard, Todd Belcher, Chip Bell, Erez Ben-Aharon, ",
                            "Jonathan Bender, Nick Bennett, Ireney Berezniak, Ted Berg, ",
                            "Gunnar Bergem, Russell Beuker, Ed Bickford, Stephen Biles, ",
                            "John Billdt, Gerald Binder, John Bird, Hannah Blackerby, Tom ",
                            "Blackerby, Clayton Blackwell, Thomas Blake, Shawn Blaszak, ",
                            "Daniel Bliss, Fred Bliss, Jeff Bliss, Jon Blum, Rune Boersjoe, ",
                            "Andrew Boggs, Dave Boisvenu, Joe Bolt, John Bonds, Jeff ",
                            "Borenstein, Dorian Borin, Ed Boris, Bartholomew Botta, ",
                            "Michael Boutin, Michael Boyd, Charles Boyer, Mike ",
                            "Boyersmith, Michael Boyko, Eric Bradberry, John Brandstetter, ",
                            "Ryan Breding, Paul Brinkmann, Patrick Briscoe, Scott Brisko, ",
                            "Jeremy Britton, Adrian Broadhead, Glenn Brouwer, Joe Brown, ",
                            "Sebastien Brulotte, Darrell Brunsch, William Bryan, Jason ",
                            "Bucher, Chris Buchinger, Clayton Buckingham, John Buckles, ",
                            "David Bugay, Ed Bujone, Erik Burgess, Gabriel Burkett, Chris ",
                            "Burnes, Richard Butler, Jeffrey Bye, Dougall Campbell, Donnie ",
                            "Cannon, Shane Cantrell, Connor Caple, Daniel Carey, James ",
                            "Carlton, Michael Carmel, Mike Carpenter, Ronald Carruthers, ",
                            "Phil Carter, Bryce Cartmill, Eric Cartwright, Anthony Caruso, ",
                            "Lon Casey, Tim Caster, Aaron Chan, Otto Chan, Gene Chang, ",
                            "Hsiao-Lung Chang, William Chang, George Chappel, Larry ",
                            "Charbonneau, Troy Chase, Bruce Chen, Chun Hsien Chen, Sam ",
                            "Chen, Tzu-Mainn Chen, Mike Cheng, Chris Chiapusio, Damien ",
                            "Chiesa, Nick Chin, Nim Ching, Jonathan Chinn, Michael Chinn, ",
                            "Philander Chiu, Thayne Christiansen, Philip Chui, Steve ",
                            "Cintamani, Richard Ciordia, Colin Clark, Steve Clark, Samuel ",
                            "Clements, Meredith Clifton, Jeff Cohen, Dale Colton, Dax ",
                            "Combe, Matt Compton, Jacob Conklin, Richard Conn, Zac ",
                            "Cook, Tim Coolong, Gregory Coomer, Dennis Cosgrove, Kelly ",
                            "Couch, Andrew Coulter, Eric Coutinho, David Coutts, James ",
                            "Craig, John Craig, Kazial Craig, John Crawford, Marcelo ",
                            "Crespo, Orville Crews, Tim Cristy, Elmer Crosby III, Russell ",
                            "Cullison, Ryan Cupples, Andrew Dagley, Steve Dallaire, Richard ",
                            "Dalton, David Dandar, Pro Daulo, Rob Dautermann, Mike ",
                            "Davies, Kalieb Davis, Marshall Davis, Ronald Davis, Danny De ",
                            "Bie, Marc De Filippis, Myles Deighton, Kent Dejarnett, Anthony ",
                            "Delarosa, Neil Dempster, Peter Denitto, Joaquim Dentz, Scott ",
                            "Dewar, Anish Dhingra, Philip Dhingra, David Diaz, Stewart ",
                            "Dicks, Brad Dietz, Josh Dietz, Colleen Diggins, Mike Ditchburn, ",
                            "Eric Dittman, Allen Do, Huy Doan, Matthew Dolman, Antoine ",
                            "Dongois, Eamonn Donohoe, Bill Dorell, Mark Dorison, Dan ",
                            "Dorsett, Chris Dorsey, Jim Dose, Willis Doss, Chin Du, William ",
                            "Dubis, Timothy Duewell, Brandon Dunn, Andrew Durham, Don ",
                            "Duvall, Kevin Dvojack, Daniel Eaton, Paul Eberting, Eric ",
                            "Echelbarger, Lance Eddleman, Ben Eggers, David Eggum, John ",
                            "Ehde, Brian Eikenberry, Patrick Elven, Peter Engdahl, Philip ",
                            "Engdahl, Michael Ennis, Darren Eslinger, Eric Ezell, Darren ",
                            "Falslev, Stephen Feather, Tony Fenn, Ben Ferguson, Mike ",
                            "Fernandez, Gwendal Feuillet, Guy Fietz, Julian Figueroa, Dale ",
                            "Fillpot, Stan Finchem, Michael Finley, Nick Fisher, William ",
                            "Fisher, Mark Fitlin, Dave Flatt, Joel J. Flores, John Folkers, ",
                            "Steven Forgie, Tom Forsythe, Matthew Foster, Scott Francis, ",
                            "Jim Frank, Paulo Fraser, Glenn French, Kurt Frerichs, Chris ",
                            "Frey, Mark Friedman, Charles Friedmann, Dan Friend, Kirk ",
                            "Fry, Aaron Fu, Erik Gaalema, Brandon Gabbard, Phil Gagner, ",
                            "Tommy Gannon, David Gappmayer, Chris Garrison, Tony ",
                            "Garrison, David Gasca, Jeremy Gasser, Michael Geist, Michael ",
                            "Genereux, Daniel Genovese, Josh Gerwin, Paul Gibson, William ",
                            "Gilchrist, Gabriel Gils Carbo, Chad Glendenin, Ryan Glinski, ",
                            "Dean Gobrecht, Andrew Goldfinch, David Goodman, Mark ",
                            "Goodson, Matt Gordon, Frank Gorgenyi, Sean Gould, Perry ",
                            "Goutsos, Ed Govednik, Michael Grayson, Chris Green, Justin ",
                            "Grenier, Jeff Greulich, Don Grey, Rob Griesbeck, Don Griffes, ",
                            "Kimberly Griffeth, Jay Grizzard, Don Gronlund, Joe Gross, ",
                            "Troy Growden, Greg Guilford, David Gusovsky, Jeremy ",
                            "Guthrie, Adam Gutierrez, James Guzicki, Matthew Haas, Matt ",
                            "Hadley, Ryan Hagelstrom, Bobby Hagen, Ben Hall, Brian Hall, ",
                            "Kris Hall, Calvin Hamilton, Kris Hamilton, Bo Hammil, Dave ",
                            "Hans, Rick Hansen, Robert Harlan, Travis Harlan, Seth ",
                            "Harman, Jeff Harris, Shawn Hartford, Adam Hartsell, Neil ",
                            "Harvey, Ray Hayes, John Hein, Chris Heinonen, Christer ",
                            "Helsing, Chris Hempel, Dustin Hempel, Mathieu Henaire, Matt ",
                            "Henry, Chuck Herb, Michael Herron, Sage Herron, Thomas ",
                            "Herschbach, Cliff Hicks, Nelson Hicks, Paul Hierling, William ",
                            "Hiers, Mike Higdon, Tim Hildebrand, Casey Hinkle, Ryan ",
                            "Hitchings, Wes Hix, Alan Ho, Jenson Ho, Alan Hoffman, Jeff ",
                            "Hoffman, Eleanor Hoffmann, Steve Hogg, Richard Holler, Brian ",
                            "Homolya, Wade Hone, Joe Horvath, Jeff Howe, Eric Hudson, ",
                            "Glen Huey, Chris Hufnagel, Joshua Hughes, Melissa Hughes, ",
                            "Arief Hujaya, Thomas Hulen, Ryan Hupp, Justin Hurst, Rick ",
                            "Hutchins, Steve Iams, Mike Iarossi, Bjorn Idren, Johan Idren, ",
                            "Micah Imparato, Joe Ingersoll, David Ingram, Greg Ipp, Rodney ",
                            "Irvin, Darin Isola, Justin Itoh, Mario Ivan, Fredrik Ivarsson, ",
                            "Dax Jacobson, Michael Jacques, Stevens Jacques, Duane Jahnke, ",
                            "William Jambrosek, Daniel Janick, Narciso Jaramillo, Neil ",
                            "Jariwala, Harvie Jarriell, Scott Javadi, Joe Jenkins, Bart ",
                            "Jennings, Paul Jennings, Julien Jenny, Jason Jensen, Martin ",
                            "Jeremy, Mark Jeschke, Andy Johnson, James Johnson, Leigh ",
                            "Johnson, Mark Johnson, Rupert Johnson, Clyde Jones, Michael ",
                            "Jones, Tim Jordan, Ben Judy, Michael Kaae, Steve Kaczkowski, ",
                            "Neville Kadwa, Brian Kaisner, Yoshihisa Kameyama, Michael ",
                            "Kanemura, Daniel Kao, Eric Karabin, Ben Katz, Christopher ",
                            "Kawamura, Erick Kayser, Craig Keddie, Kevin Kelley, Bryan ",
                            "Kemp, Michael Kendrigan, Dan Kerber, Timothy Kerber, Tomi ",
                            "Keski-Heikkila, Greg Kettering, Nathan Kilber, Howard Kim, ",
                            "Orrin Kinion, Jon Kirst, David Kitch, John Klingbeil, Neil ",
                            "Klopfenstein, Kerry Knouse, David Knox, Said Kobeissi, Jeff ",
                            "Koches, Hades Kong, Jeff Kong, Kevin Konkle, Steve Koon, ",
                            "David Koontz, Dan Koopmann, Steve Koskela, Kuan Kou, ",
                            "Cameron Kracke, Jensen Krage, York Kramer, Cedar Kraus, ",
                            "Jason Kraus, Bobby Krimen, Melissa Krispli, Steven Krispli, ",
                            "James Kruger, Charles Kubasta, Kimmo Kulonen, Frank ",
                            "Lackaff, Michael Lacour, Matt Lake, Jason Landry, Hans Erik ",
                            "Lange, Michael Laramee, Brad Lascelle, Pat Laschinger, Alan ",
                            "Lau, Sean Laurence, Anthony Lavey, Jr., Gary Le, Huey Le, ",
                            "Stephane Le Roy Audy, Lim Leandro, Charles Lee, Conroy Lee, ",
                            "Mike Lee, Shih-Hang Lee, Jonathan Leipert, Jason Lemann, ",
                            "Ron Lenzi, Mitchell Leon, Stephanie Lesniewski, Brendan ",
                            "Lewis, Robert Lewis, Sam Liao, Tom Liem, Adam Ligas, Steven ",
                            "Liggett, Roger Lilley, Benjamin Lim, Jeff Lindholm, Johnson ",
                            "Linwood, David Litchman, Bruce Lithimane, William Liu, ",
                            "Wilson Liu, Robert Lobdell, Chris Logan, Razvan Loghin, Jack ",
                            "Loh, George Loo, Russell Love, Juan Loyola, Ricardo Lozano, ",
                            "Mike Luban, Tim Luc, Henry Luciano, Dianne Ludwig, Charles ",
                            "Lueras, Derek Lung, Phong Ly, Scott MacGillivray, Dave Mack, ",
                            "Alec Mak, Steve Mamayek, Michael Mancini, Daniel Mann, ",
                            "Michael Mann, Chris Manofsky, Abdullah Marafie, Nicholas ",
                            "Marcy, Piompino Mariano, Bob Marius, Trey Marshall, Dane ",
                            "Martin, Gregg Martin, Renard Martin, Rich Martin, Scott ",
                            "Martin, Thomas Martin, Jon Masters, Christopher Mathews, ",
                            "Jay Mathis, Marc Matthews, Chris Mazur, Doug McBride, ",
                            "Mackey McCandlish, Robin McCollum, Steven McCombie, ",
                            "Andy McConnell, Michael McCourt, Bill McCoy, Doug ",
                            "McCracken, Michael McDeed, Robert McDonald, Steve ",
                            "McEachron, Craig McGee, Ryan McKenzie, Michael McKeown, ",
                            "Daniel McMahon, Colin McMillan, Ian McWilliam, Mark ",
                            "McWilliams, Khann Mean, Bryan Meason, Kenneth Medley, ",
                            "Jeff Meek, John Mehr, Christopher Mende, Brian Mendenhall, ",
                            "Peter Mengel, Michael Mersic, Mike Messom, Don Metcalf, ",
                            "Gary Metzker, Scott Meyer, Joseph Michaud, Andrew Mielke, ",
                            "Travis Mikalson, Troy Milburn, Ike Miller, Ronnie Miller, Sean ",
                            "Miller, Steve Miller, Arthur Min, David Minniti, Brenda ",
                            "Mirsberger, Bill Misek, David Mitchell, Joseph Mobley, Robert ",
                            "Mollard, Will Mooar, Curtis Moore, Matthew Moore, Al ",
                            "Morales, Ryan Moran, Lance Mortensen, Karel Mrazek, Ward ",
                            "Mullee, William Munoz, Kirk Munro, Craig Murray, Shawn P. ",
                            "Murray, Travis Murray, Michael Mushrush, Tom Mustaine, ",
                            "David Myers, Joseph Myett, Morgan Najar, Kenta Nakamura, ",
                            "Damian Nastri, Joshua Naumann, Nick Navarro, Douglas ",
                            "Neitzel, Arnold Ng, Anthony Nguyen, Steve Nguyen, Joseph ",
                            "Nicholas, Charles Nickolaus, Jon Nisbet, Patrick Nomee, David ",
                            "Norling-Christensen, Bobby Norton, Joseph Nottingham, Frank ",
                            "O'Connor, Jon Oden, David Oester, Lavern Ogden, Zach ",
                            "Oglesby, Lucas Oldfield, Toby Olsson, Aaron Ondek, Sean ",
                            "O'Neill, John Orlando, Samuel Orlando, Donovan Orloski, ",
                            "David Pai, Nikolas Paldan, David Palek, John Palmieri, Anthony ",
                            "Palmisano, Sanjay Pandit, Jesse Park, Alex Parker, Jimmy ",
                            "Pasher, Lukasz Paszek, Andy Patterson, William Pelletier, ",
                            "Duane Pemberton, Ivan Pemic, Kelly Pendergast, Mike ",
                            "Penezich, Jon Penk, Willie Penley, Ron Penna, Matthew ",
                            "Pennington, Kevin Pereira, Ross Perez, Ken Perkins, Brian ",
                            "Peterik, Kelly Peterson, Chris Phillips, Rod Pickett, Cameron ",
                            "Pierce, Reuben Pierce, Tim Pilger, Billy Pippin, Brad Plank, ",
                            "Brian Plant, Craig Platt, David Plunkett, Michael Politi, Albert ",
                            "Portillo, Brian Powell, David Powell, Franklin Powers Jr., Alan ",
                            "Precourt, Michael Pronchick, Julian Quintana, Justin Radziej, ",
                            "Steven Rajewski, Shawn Rawles, Ian Reardon, Marc Reed, Ric ",
                            "Reichelt, Judd Reiffin, David Reilly, Garry Reisky, Drew ",
                            "Ressler, Robert Reynolds, Walter Reynolds, Michael Rice, Ian ",
                            "Richards, James Richards, Raymond Richmond, Dustin Riggs, ",
                            "Keith Riskey, Brian Ro, Scott Roberts, Jorge Rodriguez, Chad ",
                            "Rogers, Clint Rogers, Robert Rogers, Steve Rogers, Ethan Roots, ",
                            "Ron Roque, William Ross, Sebastian Rossi, Jeff Rostis, Ben ",
                            "Roth, Demar Roth, Rich Rouse, Oleg Rovner, Jonathan Roy, ",
                            "Drew Rozema, Mike Ruggles, Mathias Russ, James Russell, Jim ",
                            "Rutledge, James Rutter, Dave Ryder, Chris Salvadras, Anders ",
                            "Samnerud, Nick Sanders, Jakob Sandgren, Joe Sapinsky, Tyler ",
                            "Sargent, Jonas Saunders, Mark Savage, Scott Sawyer, Robert ",
                            "Scanlon, Trevor Schaben, Aaron Schmidt, Chris Schmidt, Greg ",
                            "Schultz, Nicholas Schumacher, Scott Schumacher, Kevin Scott, ",
                            "Rastislav Seffer, Robert Seidler, Corey Sellers, Justin Sellers, ",
                            "Marc Senecal, George Shannon, Ian Sheffield, Anoop Shekar, ",
                            "Sandeep Shekar, Kevin Shelton, Leon Shephard, Eric Shepperd, ",
                            "Jeffrey Shneidman, Samuel Shockey, Mark Shoemaker, Mike ",
                            "Shupe, Sean Sibbet, Brian Sidharta, Jimmy Sieben, Eric ",
                            "Siemens, William Silva, Jody Simpson, Jatinder Singh, Sonia ",
                            "Siu, Omar Skarsvaag, Tom Skiba, Carl Skow, David Skuse, ",
                            "Robert Slifka, Brent Smith, C. Eric Smith, Jared Smith, Jeffrey ",
                            "Smith, Owen Smith, Shannon Smith, Steven Smith, Edward Smola, ",
                            "Matthew Sneep, Eric Snow, Brad Sobel, Jean-Pierre Solignac, Rasmus ",
                            "Sorensen, Andrew Sorg, Poppy Southcott, Ross Specter, Erik Spencer, ",
                            "Keith Spencer, Chris Springer, Erikson Squier, Dean St. Onge, Stewart ",
                            "Stanfield, John Stanley, Terrence Staton, Benjamin Stein, James ",
                            "Steiner, David Steyer, William Stickney, Chris Stiff, James Stofer, ",
                            "Norm Storch, Patrick Stovall, Brandon Sturgeon, Sean Stutler, Anne ",
                            "Sukprasert, Jamal Sullivan, Bruce Sully, Jay Sung, Park Sung ",
                            "Joon, Stein Sunnarvik, Nathan Surginer, Robert Swaringen, Lee ",
                            "Sweeney, David Szabo, Scott Taft, Christian Takvam, Patrick ",
                            "Talaska, Tadashi Tamaki, Simon Tan, Mark Taraba, Khon-",
                            "Whey Tay, John Taylor, Keith Taylor, Donald Temean, Phillip ",
                            "Tesar, Pete Thao, Brian Thomas, Keith Thomas, Speed Thomas, ",
                            "Miles Thorpe, Paul Thurrott, Mike Timbol, Nicholas Timmins, ",
                            "Tom Tobin, Robert Towster, Hien Tran, Timothy Traviss, Toby ",
                            "Traylor, Tony Treadwell, George Tremoulis, Paul Trinh, Thanh ",
                            "Trinh, Chris Tristan, Brad Truswell, Jason Tryon, Mike Tu, ",
                            "Gernel Tuazon, Eric Tuggle, Mike Turnbull, Lyle Ubben, ",
                            "Amilcar Ubiera, Robert Ulozas, Arie Upton, Mark Van Noy, ",
                            "Matthew Van Sickler, Jake Vantlin, Tony Vasquez, Brady ",
                            "Vauclin, Gianpiero Vecchi, Chad Verrall, Chris Vicente, Brett ",
                            "Vickers, David Vickery, Jonathan Vilante, Simon Vince, Ben ",
                            "Vinson, David Voelkert, Paul Vogt, Nicholas Voorhies, Robert ",
                            "Vreeland, Mike Vrooman, Rick Vuong, Brian Wachhaus, Todd ",
                            "Wachhaus, Caine Wade, Mathew Wadstein, Kenneth Wagenius, ",
                            "Trevor Wagner, John Wagstaff, Asad Wahid, Richard ",
                            "Wahlberg, Helio Wakasugui, Richard Walker, Wilkins Walker, ",
                            "Matthew Wallace, Daniel Walsh, Joel Walters, Andrew Waltz, ",
                            "Tom Wang, Tony Wang, Jay Ward, Jonathan Ward, John ",
                            "Warner, Mark Warren, Matt Washer, Mike Watson, Sean ",
                            "Wattles, Mike Wayne, Christian Wehba, Benjamin Wei, Richard ",
                            "Weight, Cary Wells, David Wenck, Bill Werring, Leonard ",
                            "Wesley, Marlon West, Mikael Westerbacka, Brian Wharry, ",
                            "Chris White, Chris White, Jeremy White, Greg Whitlock, Gary ",
                            "Widener, Marty Wilfried, Israel Wilkinson, Michael Willams, ",
                            "Derek Williams, Sean Willson, Nitzan Wilnai, Jim Wilson, ",
                            "Karsten Wilson, William Wilt, Tim Winn, Brian Winzeler, Matt ",
                            "Wise, Lee Wissmiller, Brendan Wolfe, Daniel Wolpert, Felix ",
                            "Wong, Power Wong, Tony Wong, Tim Wood, Timothy Wood, ",
                            "Jeremy Woods, Michael Woods, Bill Wright, Keith Wright, ",
                            "Patrick Wu, Gang Xie, Robert Yao, Sun Lim Yap, Stephen Yau, ",
                            "Erek Yedwabnick, Christopher Yee, Nick Yee, Juan Yip, David ",
                            "Young, Rob Young, Seth Young, Alex Yu, Terry Zahn, Jia-Ning ",
                            "Zhang, Eric Zieg, Jordan Zielin, Clint Zimmerman, Matt Zinke ",
                            "",
                            "",
                            "",
                            "",
                            "$No souls were sold in the making of this game.",
                            "",
                            "",
                            " ",
                            " ",
                            " ",
                            NULL};

int creditline = 0;
int ybase = 0;
int linecount = 24;
int nottheend = 1;

/////////////////////////////////////////

void FreeMenuItems()
{

	SDL_FreeSurface(MainMenuItemsSurface);
	SDL_FreeSurface(MenuSelectNewHeroSurface);
	SDL_FreeSurface(CreateHeroDialogSurface);
	SDL_FreeSurface(CreateHeroDialogSurface);
	SDL_FreeSurface(CursorImg);
	SDL_FreeSurface(DiabloTitle);

	SDL_DestroyTexture(MainMenuItemsTexture);
	SDL_DestroyTexture(DiablologoAnimT);
	SDL_DestroyTexture(CursorTexture);
	SDL_DestroyTexture(MenuSelectNewHeroTexture);
	SDL_DestroyTexture(CreateHeroDialogTextureW);
	SDL_DestroyTexture(CreateHeroDialogTextureR);
	SDL_DestroyTexture(CreateHeroDialogTextureS);
}

char gLDirectory[FILENAME_MAX];
void GetWorkingLocationOfFile(char *RelativeFile)
{
	GetCurrentDir(gLDirectory, FILENAME_MAX);
	strcat(gLDirectory, RelativeFile);
}

uint32_t XgetTick()
{

	unsigned theTick = 0U;
	printf("This is supposed to replace GitTicks()");
	return theTick;
}

void SDLCreateDiabloCursor() {}

void SdlDiabloMainWindow()
{

	SDL_Init(SDL_INIT_EVERYTHING);
	//IMG_Init(IMG_INIT_PNG);
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Diablo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Window_Width, Window_Height,
	                          SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);
	printf("Window And Renderer Created!\n");


	SDL_RenderSetLogicalSize(renderer, 800, 600);
	const int pitch = SCREEN_WIDTH + 64 + 64;

	surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	assert(surface);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	assert(texture);

	palette = SDL_AllocPalette(256);

	lock_buf_priv();
}

void DrawArtImage(int SX, int SY, int SW, int SH, int nFrame, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer + (SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for (int i = 0; i < SH; i++, src += SW, dst += 768) {
		for (int j = 0; j < SW; j++) {
			dst[j] = src[j];
		}
	}
}

void __fastcall print_title_str_large(int a1, int a2, char *a3)
{
	assert(BigTGold_cel);
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for (i = 0; v7 > i; ++i) {
		v5 = lfontframe[fontidx[a3[i]]];
		if (v5)
			CelDecodeOnly(screen_x, screen_y, BigTGold_cel, v5, 46);
		screen_x += lfontkern[v5] + 2;
	}
	gb_Lfont_pix_width = screen_x;
	gb_Lfont_str_len = v7;
}

void __fastcall print_title_str_small(int a1, int a2, char *a3)
{
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for (i = 0; i < v7; ++i) {
		v5 = mfontframe[fontidx[a3[i]]];
		if (v5)
			CelDecodeOnly(screen_x, screen_y, pMedTextCels, v5, 22);
		screen_x += mfontkern[v5] + 2;
	}
}

void LoadDiabloMenuLogoImage() {}

void DiabloMainMenuItemsLoaded() {}

void CreateMainDiabloMenu() {}

void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer + (SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for (int i = 0; i < SH; i++, src += SW, dst += 768) {
		for (int j = 0; j < SW; j++) {
			if (src[j] != bMask)
				dst[j] = src[j];
		}
	}
}

void DrawPCXString(int x, int y, int w, int h, char *str, BYTE *font, void *pBuff)
{
	int i;
	int len = 0;
	for (i = 0; i < strlen(str); i++) {
		DrawArtWithMask(x + len, y, w, h, str[i], 32, pBuff);
		// DrawPCX(x+len, y, str[i], 32);
		if (font[str[i] + 2])
			len += font[str[i] + 2];
		else
			len += *font;
	}
}

int __fastcall GetPCXFontWidth(char *str, BYTE *font)
{
	int len; // eax
	unsigned __int8 i; // bl
	BYTE chr; // bl
	int width; // esi

	len = 0;
	for (i = *str; *str; i = *str) {
		chr = font[i + 2];
		if (chr)
			width = chr;
		else
			width = *font;
		len += width;
		++str;
	}
	return len;
}

void ShowCredts()
{

/*

	int diablogo_cel_frame = 1;

	int MyPcxDelay = 60;
	int MyPcxFRAME = (SDL_GetTicks() / MyPcxDelay) % 15;
	if (++MyPcxFRAME == 29) {
		MyPcxFRAME = 1;
	}


*/




	ybase += 1;
	if (ybase >= pFont2[1]) {
		ybase = 0;
		if (!nottheend) {
			linecount--;
			creditline++;
		} else if (the_long_credits[creditline + 24] == NULL) {
			nottheend = 0;
			// linecount--;
		} else
			creditline++;
	}

	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	//	DrawArtImage(0, 0, gdwCreditsWidth, gdwCreditsHeight, 0, pPcxCreditsImage);
    

	for (int i = 0; i < linecount; i++) {
    
        
        //int ticks = SDL_GetTicks() / 60) % 15;


		// Needs to be slower...
		if (*the_long_credits[creditline + i] == '$'  ){


			DrawPCXString(320 - (GetPCXFontWidth(the_long_credits[creditline + i] + 1, pFont2) / 2),
			              50 + (i * pFont2[1]) - ybase, gdwFont2Width, gdwFont2Height,
			              the_long_credits[creditline + i] + 1, pFont2, pPcxFont2Image);
        }
		else{
			DrawPCXString(320 - (GetPCXFontWidth(the_long_credits[creditline + i], pFont3) / 2),
			              50 + (i * pFont3[1]) - ybase, gdwFont3Width, gdwFont3Height, the_long_credits[creditline + i],
			              pFont3, pPcxFont2Image);
	}

    }
}

///////////////////////////Renders

void RenderDiabloLogo()
{
	int diablogo_cel_frame = 1;

	int MyPcxDelay = 60;
	int MyPcxFRAME = (SDL_GetTicks() / MyPcxDelay) % 15;
	if (++MyPcxFRAME == 29) {
		MyPcxFRAME = 1;
	}

	CelDecodeOnly(64, 360, GameTitle, MyPcxFRAME, 640);

	//	DrawArtWithMask(320 - (gdwLogoWidth / 2), 0, gdwLogoWidth, gdwLogoHeight, MyPcxFRAME, 250, pPcxLogoImage);

	// if (++diablogo_cel_frame == 29) {
	// 	diablogo_cel_frame = 1;
	// }
}

void DrawCursor(int mx, int my)
{
	SDL_GetMouseState(&mx, &my);

	int lines = gdwCursorWidth;

	CelDecodeOnly(403, 326, pPcxCursorImage, 1, 36);

	//	DrawArtWithMask(mx, my, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	unlock_buf_priv();
}

void DrawMouse()
{

	int lines = gdwCursorHeight;
	// if(MouseY > 480-gdwCursorHeight)
	// 	lines -= (MouseY - (480-gdwCursorHeight));
	// int mx = MouseX;
	// if(mx < 0) mx = 0;
	// if(mx >639) mx = 639;
	// int my = MouseY;
	// if(my < 0) my = 0;
	// if(my > 480) my = 480;

	SDL_GetMouseState(&MouseX, &MouseY);

    //CelDecodeOnly(64, 64, pCursCels, 1, 28);

	//CelDecodeOnly(MouseX, MouseY, pCursCels, 1, 36);

   // frame_width = InvItemWidth[frame];

    

	//	lock_buf_priv();
	// DrawArtWithMask(MouseX, MouseY, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	//	unlock_buf_priv();
}

void SDL_RenderDiabloMainPage()
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);

	int totalPentFrames = 9;
	int PentdelayPerFrame = 60;
	int Pentframe = (SDL_GetTicks() / PentdelayPerFrame) % totalPentFrames;

	if (++Pentframe == 9) {
		Pentframe = 1;
	}

	int PentPositionX1 = (SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.28);
	int PentPositionY1 = (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.2);
	int PentPositionX2 = (SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.75);
	int PentPositionY2 = (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.2);

	// scrollrt_draw_cursor_back_buffer(); // Doesn't work?

	RenderDiabloLogo();

	// print_title_str_large();
	gmenu_print_text((SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.2), (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.2),
	                 "Single Player");
	gmenu_print_text((SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.2), (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.3),
	                 "Multi Player");
	// gmenu_print_text((SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.2), (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.3),  "Show
	// Replay");
	gmenu_print_text((SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.2), (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.4),
	                 "Play Credits");
	gmenu_print_text((SCREEN_WIDTH / 2) - (SCREEN_WIDTH * 0.2), (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT * 0.5),
	                 "Exit  Diablo");

	CelDecodeOnly(PentPositionX1, PentPositionY1, MenuPentegram, (int)Pentframe, 48);
	CelDecodeOnly(PentPositionX2, PentPositionY2, MenuPentegram, (int)Pentframe, 48);

	ADD_PlrStringXY(0, 600 - 150, 640, "DedicaTed To David Brevik, Erich Schaefer, Max Schaefer,", COL_RED);
	ADD_PlrStringXY(0, 600 - 130, 640, " MaTT Uelman, and The Blizzard North Team ThaT Gave Us A Childhood.", COL_RED);
}

void SDL_RenderDiabloSinglePlayerPage()
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	// DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	RenderDiabloLogo();
	RenderCharNames();
}

void LoadFont() {}

void LoadClickBoxes(int numberofchars)
{
	SDL_Rect Charpos1;
	Charpos1.x = -1;
	Charpos1.y = -1;
	Charpos1.h = -1;
	Charpos1.w = -1;

	SDL_Rect Charpos2;
	Charpos2.x = -1;
	Charpos2.y = -1;
	Charpos2.h = -1;
	Charpos2.w = -1;

	SDL_Rect Charpos3;
	Charpos3.x = -1;
	Charpos3.y = -1;
	Charpos3.h = -1;
	Charpos3.w = -1;

	SDL_Rect Charpos4;
	Charpos4.x = -1;
	Charpos4.y = -1;
	Charpos4.h = -1;
	Charpos4.w = -1;

	SDL_Rect Charpos5;
	Charpos5.x = -1;
	Charpos5.y = -1;
	Charpos5.h = -1;
	Charpos5.w = -1;

	SDL_Rect Charpos6;
	Charpos6.x = -1;
	Charpos6.y = -1;
	Charpos6.h = -1;
	Charpos6.w = -1;

	// The menu doesn't fit past 6 chars.
	SDL_Rect Charpos7;
	SDL_Rect Charpos8;
	SDL_Rect Charpos9;
}

int LoadedFont = 0;
int TotalPlayers = 0;

void DrawNewHeroKartinka(int image, int ShowClasses)
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	// DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	RenderDiabloLogo();

	char *heroclasses[3] = {"Warrior", "Rogue", "Sorceror"};

	// this should not be hard coded.
	int x = 280;
	int y = 430;
	// DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, image, pPcxHeroImage);

	if (ShowClasses == 1) {
		for (int i = 0; i < 3; i++) {
			y += 40;
			// print_title_str_small(x, y, heroclasses[i]);
			gmenu_print_text(x, y, heroclasses[i]);
		}
	}
}


void DrawPreGameOptions(int image, int ShowClasses)
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	// DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	RenderDiabloLogo();

	char *GameOptions[2] = {"New Game", "Load Game"};

	// this should not be hard coded.
	int x = 280;
	int y = 430;
	// DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, image, pPcxHeroImage);

	if (ShowClasses == 1) {
		for (int i = 0; i < 2; i++) {
			y += 40;
			// print_title_str_small(x, y, heroclasses[i]);
			gmenu_print_text(x, y, GameOptions[i]);
		}
	}
}


void DrawPreGameDifficultySelection(int image, int ShowClasses)
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	// DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	RenderDiabloLogo();

	char *GameOptions[3] = {"Normal", "Nightmare", "Hell"};

	// this should not be hard coded.
	int x = 280;
	int y = 430;
	// DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, image, pPcxHeroImage);

	if (ShowClasses == 1) {
		for (int i = 0; i < 3; i++) {
			y += 40;
			// print_title_str_small(x, y, heroclasses[i]);
			gmenu_print_text(x, y, GameOptions[i]);
		}
	}
}












void RenderDefaultStats(int HeroChosen)
{
	int x = 80;
	int y = 530;
	char *WarriorStats[4] = {"Strenght : 30", "Magic : 10", "Dexterity : 20", "Vitality : 25"};
	char *RogueStats[4] = {"Strenght : 20", "Magic : 15", "Dexterity : 30", "Vitality : 20"};
	char *SorcerorStats[4] = {"Strenght : 15", "Magic : 35", "Dexterity : 15", "Vitality : 20"};

	if (HeroChosen == 0) {
		print_title_str_small(x, y - 20, "Warrior Stats:");
		for (int i = 0; i < 4; i++) {
			print_title_str_small(x, y, WarriorStats[i]);
			// DrawPCXString(x, y, gdwFont3Width, gdwFont3Height, WarriorStats[i], pFont3, pPcxFont3Image);
			y += 20;
		}
	}
	if (HeroChosen == 1) {
		print_title_str_small(x, y - 20, "Rogue Stats:");
		for (int i = 0; i < 4; i++) {
			print_title_str_small(x, y, RogueStats[i]);
			// DrawPCXString(x, y, gdwFont3Width, gdwFont3Height, RogueStats[i], pFont3, pPcxFont3Image);
			y += 20;
		}
	}
	if (HeroChosen == 2) {
		print_title_str_small(x, y - 20, "Sorceror Stats:");

		for (int i = 0; i < 4; i++) {
			print_title_str_small(x, y, SorcerorStats[i]);
			// DrawPCXString(x, y, gdwFont3Width, gdwFont3Height, SorcerorStats[i], pFont3, pPcxFont3Image);
			y += 20;
		}
	}
}

void RenderUndecidedHeroName()
{
	gmenu_print_text(270, 450, (char *)HeroUndecidedName);
}

void SetHeroStats(_uiheroinfo *a1)
{
	memcpy(&heroarray[TotalPlayers], a1, sizeof(_uiheroinfo));
}

void LoadHeroStats()
{
	pfile_ui_set_hero_infos(SetHeroStats);
}

void DrawHeroStats()
{

	int x = 80;
	int y = 415;

	/*
	Render charactor stats if you want.

	*/
}

void RenderCharNames()
{
	// 	const char *hero_name = hero_infos[0].name;
	// DUMMY_PRINT("use hero: %s", hero_name);
	// strcpy(name, hero_name);
	// *dlgresult = 2;

	// X 355, Y 269
	int x = 350;
	int y = 430;
	TotalPlayers = 0;
	for (int i = 0; i < 6; i++) {
		if (hero_names[i][0] != 0) {
			// Checking if section of array is empty.
			// if array has something in this then draw name.
			print_title_str_small(x, y, hero_names[i]);

			y += 35;
			TotalPlayers++;
		}
		if (TotalPlayers < 6 || TotalPlayers == 0) {
			print_title_str_small(305, 620, "New Hero");
		}
	}

	// SDL_RenderPresent(renderer);
}

void ConstantButtons()
{
	print_title_str_small(500, 620, "Ok");
	print_title_str_small(600, 620, "Cancel");
}

bool LoadCreateHeroDialogImages = 0;
bool SorcerorCreateSelected = 0;
bool RogueCreateSelected = 0;
bool WarriorCreateSelected = 1;

void LoadCreateHeroDialogMenu() {}
// Have this load the function above and then render it in the main menu.
// Cnacel box is also needed.
void CreateHeroMenu()
{
	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	// DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	// DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	RenderDiabloLogo();
}
