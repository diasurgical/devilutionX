#include "../types.h"
#include "miniwin_sdl.h"
#include "stubs.h"

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

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
bool TitleImageLoaded = false;

void *pPcxLogoImage;
int gdwLogoWidth;
int gdwLogoHeight;
void *pPcxLogoSmImage;
int gdwLogoSmWidth;
int gdwLogoSmHeight;

int gdwTitleWidth;
int gdwTitleHeight;
void *pPcxTitleImage;

int gdwCursorWidth;
int gdwCursorHeight;
void *pPcxCursorImage;

int gdwHeroHeight;
int gdwHeroWidth;
void *pPcxHeroImage;

int gdwSHeroHeight;
int gdwSHeroWidth;

int gdwFont16Width;
int gdwFont16Height;
void *pPcxFont16sImage;
void *pPcxFont16gImage;
unsigned char *pFont16;

int gdwFont24Width;
int gdwFont24Height;
void *pPcxFont24sImage;
void *pPcxFont24gImage;
unsigned char *pFont24;

int gdwFont30Width;
int gdwFont30Height;
void *pPcxFont30sImage;
void *pPcxFont30gImage;
unsigned char *pFont30;

int gdwFont42Width;
int gdwFont42Height;
void *pPcxFont42gImage;
void *pPcxFont42yImage;
unsigned char *pFont42;

void *GameTitle;
int GameTitleHeight;
int GameTitleWidth;

void *TitleMenuText;
void *MenuPentegram16;
void *MenuPentegram;
void *MenuPentegram42;

void *pDiabfrCel;

char HeroUndecidedName[17];

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

////////////////
// new vars

int gb_Lfont_str_len;
int gb_Lfont_pix_width;
int the_pcx_frame;

char *the_long_credits[] = {
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
	NULL
};

int creditline = 0;
int ybase = 0;
int linecount = 24;
int nottheend = 1;

/////////////////////////////////////////
PALETTEENTRY pcxPal[256];

void __fastcall LoadPalInMem(PALETTEENTRY *pPal)
{
	int i; // eax

	for (i = 0; i < 256; i++) {
		orig_palette[i].peFlags = 0;
		orig_palette[i].peRed = pPal[i].peRed;
		orig_palette[i].peGreen = pPal[i].peGreen;
		orig_palette[i].peBlue = pPal[i].peBlue;
	}
}

BOOL __cdecl LoadArtImage(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
	DWORD width;  // [esp+44h] [ebp-8h]
	DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

	if (!SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0))
		return 0;
	*pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
	if (!SBmpLoadImage(pszFile, NULL, *pBuffer, height * width, 0, 0, 0))
		return 0;
	if (pBuffer && data) {
		data[0] = width;
		data[1] = height / frames;
	}
	return 1;
}

BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
	DWORD width;  // [esp+44h] [ebp-8h]
	DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

	if (!SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0))
		return 0;
	*pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
	if (!SBmpLoadImage(pszFile, pcxPal, *pBuffer, height * width, 0, 0, 0))
		return 0;

	LoadPalInMem(pcxPal);

	//lpDDPalette->SetEntries(0, 0, 256, orig_palette);

	if (pBuffer && data) {
		data[0] = width;
		data[1] = height / frames;
	}
	return 1;
}

BOOL __stdcall SBmpLoadImage(const char *pszFileName, PALETTEENTRY *pPalette, void *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp)
{
	char *v7;                      // ebx
	unsigned char *v8;             // edi
	PALETTEENTRY *v9;              // esi
	int v10;                       // esi
	signed int v11;                // ebx
	int v12;                       // ebp
	size_t v13;                    // ebp
	unsigned char *v14;            // eax
	unsigned char *v15;            // edx
	int v16;                       // ebp
	unsigned char v17;             // cl
	unsigned char v18;             // al
	int v19;                       // ecx
	bool v20;                      // zf
	bool v21;                      // sf
	unsigned char(*v22)[3];        // eax
	BYTE v23;                      // cl
	unsigned char *Memory;         // [esp+14h] [ebp-38Ch]
	HANDLE hFile;                  // [esp+18h] [ebp-388h] MAPDST
	DWORD v28;                     // [esp+1Ch] [ebp-384h]
	PCXHeader pcxhdr;              // [esp+20h] [ebp-380h]
	unsigned char paldata[256][3]; // [esp+A0h] [ebp-300h]
	int z;

	if (pdwWidth)
		*pdwWidth = 0;
	if (dwHeight)
		*dwHeight = 0;
	if (pdwBpp)
		*pdwBpp = 0;
	v7 = (char *)pszFileName;
	if (pszFileName) {
		if (*pszFileName) {
			v8 = (unsigned char *)pBuffer;
			if (!pBuffer || dwBuffersize) {
				v9 = pPalette;
				if (pPalette || pBuffer || pdwWidth || dwHeight) {
					if (SFileOpenFile(pszFileName, &hFile)) {
						if (strchr(pszFileName, 92)) {
							do
								v7 = strchr(v7, 92) + 1;
							while (strchr(v7, 92));
						}
						for (; strchr(v7 + 1, 46); v7 = strchr(v7, 46))
							;
						if (!v7 || _strcmpi(v7, ".pcx")) // omit all types except PCX
						{
							return 0;
							//v10 = sub_15001C70(hFile, pPalette, pBuffer, dwBuffersize, pdwWidth, dwHeight, pdwBpp);
							//goto LABEL_51;
						}
						if (!SFileReadFile(hFile, &pcxhdr, 128u, 0, 0)) {
							v10 = 0;
						LABEL_51:
							SFileCloseFile(hFile);
							return v10;
						}
						*(_DWORD *)&paldata[0][0] = pcxhdr.xmax - pcxhdr.xmin + 1;
						v11 = pcxhdr.ymax - pcxhdr.ymin + 1;
						v28 = pcxhdr.bitsPerPixel;
						if (pdwWidth)
							*pdwWidth = *(_DWORD *)&paldata[0][0];
						if (dwHeight)
							*dwHeight = v11;
						if (pdwBpp)
							*pdwBpp = v28;
						if (!pBuffer) {
							SFileSetFilePointer(hFile, 0, 0, 2);
							goto LABEL_45;
						}
						v12 = SFileGetFileSize(hFile, 0);
						v13 = v12 - SFileSetFilePointer(hFile, 0, 0, 1);
						v14 = (unsigned char *)SMemAlloc(v13, "SBMP.CPP", 171);
						Memory = v14;
						if (!v14) {
						LABEL_45:
							if (pPalette && v28 == 8) {
								SFileSetFilePointer(hFile, -768, 0, 1);
								SFileReadFile(hFile, paldata, 768u, 0, 0);
								v22 = paldata;
								for (z = 0; z < 256; z++) {
									v23 = *(_BYTE *)v22;
									++v9;
									++v22;
									v9[-1].peRed = v23;
									v9[-1].peGreen = (*v22)[-2];
									v9[-1].peBlue = (*v22)[-1];
									v9[-1].peFlags = 0;
								}
							}
							v10 = 1;
							goto LABEL_51;
						}
						SFileReadFile(hFile, v14, v13, 0, 0);
						v15 = Memory;
						if (v11 <= 0)
							goto LABEL_43;
					LABEL_33:
						v16 = *(_DWORD *)&paldata[0][0];
						while (1) {
							v17 = *v15++;
							if (v17 < 0xC0u) {
								*v8++ = v17;
								--v16;
							} else {
								v18 = *v15;
								v19 = v17 & 0x3F;
								++v15;
								for (; v19; --v16) {
									v20 = v16 == 0;
									v21 = v16 < 0;
									if (!v16)
										goto LABEL_41;
									*v8++ = v18;
									--v19;
								}
							}
							v20 = v16 == 0;
							v21 = v16 < 0;
						LABEL_41:
							if (v21 || v20) {
								if (!--v11) {
								LABEL_43:
									SMemFree(Memory, "SBMP.CPP", 178);
									goto LABEL_45;
								}
								goto LABEL_33;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

void FreeMenuItems()
{
	void *tmp;

	tmp = pPcxFont42yImage;
	pPcxFont42yImage = NULL;
	mem_free_dbg(tmp);
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

void SdlDiabloMainWindow()
{
	SDL_ShowCursor(SDL_DISABLE);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Diablo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	renderer = SDL_CreateRenderer(window, -1, 0);
	printf("Window And Renderer Created!\n");

	SDL_RenderSetLogicalSize(renderer, 640, 480);
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
void LoadDiabloMenuLogoImage()
{
}

void DiabloMainMenuItemsLoaded()
{
}

void CreateMainDiabloMenu()
{
}

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

int GetCenterOffset(int w, int bw = 0)
{
	if (bw == 0) {
		bw = SCREEN_WIDTH;
	}

	return bw / 2 - w / 2;
}

void DrawPCXString(int x, int y, int w, int h, BYTE *str, BYTE *font, void *pBuff)
{
	int i;
	int len = 0;
	BYTE chr;
	for (i = 0; i < strlen(str); i++) {
		DrawArtWithMask(x + len, y, w, h, str[i], 32, pBuff);
		chr = font[str[i] + 2];
		if (chr)
			len += chr;
		else
			len += *font;
	}
}

int __fastcall GetPCXFontWidth(char *str, BYTE *font)
{
	int i;
	int len = 0;
	BYTE chr;
	for (i = 0; i < strlen(str); i++) {
		chr = font[str[i] + 2];
		if (chr)
			len += chr;
		else
			len += *font;
	}

	return len;
}

int TextAlignment(char *text, TXT_JUST align, int bw, BYTE *pFont)
{
	if (align != JustLeft) {
		int w = GetPCXFontWidth(text, pFont);
		if (align == JustCentre) {
			return GetCenterOffset(w, bw);
		} else if (align == JustRight) {
			return bw - w;
		}
	}

	return 0;
}

void PrintText16Gold(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont16);

	DrawPCXString(x, y, gdwFont16Width, gdwFont16Height, text, pFont16, pPcxFont16gImage);
}

void PrintText16Silver(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont16);

	DrawPCXString(x, y, gdwFont16Width, gdwFont16Height, text, pFont16, pPcxFont16sImage);
}

void PrintText24Gold(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont24);

	DrawPCXString(x, y, gdwFont24Width, gdwFont24Height, text, pFont24, pPcxFont24gImage);
}

void PrintText24Silver(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont24);

	DrawPCXString(x, y, gdwFont24Width, gdwFont24Height, text, pFont24, pPcxFont24sImage);
}

void PrintText30Gold(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont30);

	DrawPCXString(x, y, gdwFont30Width, gdwFont30Height, text, pFont30, pPcxFont30gImage);
}

void PrintText30Silver(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont30);

	DrawPCXString(x, y, gdwFont30Width, gdwFont30Height, text, pFont30, pPcxFont30sImage);
}

void PrintText42Gold(int x, int y, char *text, TXT_JUST align = JustLeft, int bw = 0)
{
	x += TextAlignment(text, align, bw, pFont42);

	DrawPCXString(x, y, gdwFont42Width, gdwFont42Height, text, pFont42, pPcxFont42gImage);
}

void LoadTitelArt(char *pszFile)
{
	DWORD dwData[2];

	if (TitleImageLoaded) {
		return;
	}

	LoadArtWithPal(pszFile, &pPcxTitleImage, 1, dwData);
	gdwTitleWidth = dwData[0];
	gdwTitleHeight = dwData[1];
	TitleImageLoaded = true;
}

void ShowCredts()
{
	LoadTitelArt("ui_art\\credits.pcx");
	ybase += 1;
	if (ybase >= pFont16[1]) {
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

	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);

	for (int i = 0; i < linecount; i++) {
		// Needs to be slower...
		if (*the_long_credits[creditline + i] == '$') {
			PrintText16Gold(0, 50 + (i * pFont16[1]) - ybase, the_long_credits[creditline + i] + 1, JustCentre);
		} else {
			PrintText16Gold(0, 50 + (i * pFont16[1]) - ybase, the_long_credits[creditline + i], JustCentre);
		}
	}
}

///////////////////////////Renders

void AnimateDiabloLogo(int t, int w, int h, void *pBuffer)
{
	int MyPcxDelay = 60;
	int MyPcxFRAME = (SDL_GetTicks() / MyPcxDelay) % 15;
	MyPcxFRAME++;
	if (MyPcxFRAME == 15) {
		MyPcxFRAME = 0;
	}

	DrawArtWithMask(GetCenterOffset(w), t, w, h, MyPcxFRAME, 250, pBuffer);
}

void RenderDiabloLogo()
{
	AnimateDiabloLogo(182, gdwLogoWidth, gdwLogoHeight, pPcxLogoImage);
}

void RenderDiabloLogoSm()
{
	AnimateDiabloLogo(0, gdwLogoSmWidth, gdwLogoSmHeight, pPcxLogoSmImage);
}

void DrawCursor(int mx, int my)
{
	SDL_GetMouseState(&mx, &my);

	int lines = gdwCursorWidth;

	DrawArtWithMask(mx, my, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	unlock_buf_priv();
}

void DrawMouse()
{

	SDL_GetMouseState(&MouseX, &MouseY);
	int lines = gdwCursorHeight;
	// if(MouseY > 480-gdwCursorHeight)
	// 	lines -= (MouseY - (480-gdwCursorHeight));
	// int mx = MouseX;
	// if(mx < 0) mx = 0;
	// if(mx >639) mx = 639;
	// int my = MouseY;
	// if(my < 0) my = 0;
	// if(my > 480) my = 480;

	// frame_width = InvItemWidth[frame];

	//	lock_buf_priv();
	DrawArtWithMask(MouseX, MouseY, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	//	unlock_buf_priv();
}

void AnimateSelector(int x, int y, int width, int padding, int spacing, int swidth, void *pBuffer)
{
	int Pentframe = (SDL_GetTicks() / 60) % 8;

	Pentframe++;
	if (Pentframe == 8) {
		Pentframe = 0;
	}
	Pentframe = 0;

	width = width ? width : SCREEN_WIDTH;
	x += GetCenterOffset(swidth, width);
	y += (SelectedItem - 1) * spacing;

	DrawArtWithMask(x - width / 2 + padding, y, swidth, swidth, Pentframe, 250, pBuffer);
	DrawArtWithMask(x + width / 2 - padding, y, swidth, swidth, Pentframe, 250, pBuffer);
}

void DrawSelector16(int x, int y, int width, int padding, int spacing)
{
	AnimateSelector(x, y, width, padding, spacing, 20, MenuPentegram16);
}

void DrawSelector(int x, int y, int width, int padding, int spacing)
{
	AnimateSelector(x, y, width, padding, spacing, 30, MenuPentegram);
}

void DrawSelector42(int x, int y, int width, int padding, int spacing)
{
	AnimateSelector(x, y, width, padding, spacing, 42, MenuPentegram42);
}

void RenderDiabloSplashPage()
{
	LoadTitelArt("ui_art\\title.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);

	PrintText24Silver(-1, 410, "Copyright \xA9 1996-2001 Blizzard Entertainment", JustCentre);
	RenderDiabloLogo();
}

void RenderDiabloMainPage()
{
	char *pszFile = "ui_art\\mainmenu.pcx";
	if (false) //DiabloUI_GetSpawned()
		pszFile = "ui_art\\swmmenu.pcx";
	LoadTitelArt(pszFile);

	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);

	// scrollrt_draw_cursor_back_buffer(); // Doesn't work?

	RenderDiabloLogoSm();

	int menuTop = 192;
	char *MENIITEMS[5] = { "Single Player", "Multi Player", "Replay Intro", "Show Credits", "Exit Diablo" };

	for (int i = 0; i < 5; i++) {
		int y = menuTop + i * 43;
		if (i > 1) {
			y -= 1; // "Multi Player" and "Replay Intro" has a smaller gap then other items
		}
		PrintText42Gold(-1, y, MENIITEMS[i], 1);
	}

	int selectorTop = menuTop;
	if (SelectedItem > 2) {
		selectorTop -= 1; // "Multi Player" and "Replay Intro" has a smaller gap then other items
	}

	DrawSelector42(0, selectorTop, 0, 85, 43);

	PrintText16Silver(17, 444, gszProductName);

	ADD_PlrStringXY(0, 600 - 150, 640, "DedicaTed To David Brevik, Erich Schaefer, Max Schaefer,", COL_BLUE);// Red isn't red
	ADD_PlrStringXY(0, 600 - 130, 640, " MaTT Uelman, and The Blizzard North Team ThaT Gave Us A Childhood.", COL_BLUE);
}

void RenderStats(char *lvl, char *str, char *mag, char *dex, char *vit)
{
	PrintText16Silver(31, 323, "Level:", JustRight, 118);
	PrintText16Silver(149, 323, lvl, JustCentre, 61);
	PrintText16Silver(31, 358, "Strength:", JustRight, 118);
	PrintText16Silver(149, 358, str, JustCentre, 61);
	PrintText16Silver(31, 380, "Magic:", JustRight, 118);
	PrintText16Silver(149, 380, mag, JustCentre, 61);
	PrintText16Silver(31, 401, "Dexterity:", JustRight, 118);
	PrintText16Silver(149, 401, dex, JustCentre, 61);
	PrintText16Silver(31, 422, "Vitality:", JustRight, 118);
	PrintText16Silver(149, 422, vit, JustCentre, 61);
}

void RenderDiabloSinglePlayerPage()
{
	LoadTitelArt("ui_art\\selhero.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	RenderDiabloLogoSm();

	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, 3, pPcxHeroImage);

	PrintText30Silver(-1, 161, "Single Player Characters", JustCentre);

	int w = 369;
	int x = 241;

	PrintText30Silver(x - 1, 211, "Select Hero", JustCentre, w);

	int selectorTop = 256;
	int y = selectorTop;
	for (int i = 0; i < 0; i++) {
		PrintText24Gold(x - 1, y, "Hero name", JustCentre, w);
		y += 26;
	}
	PrintText24Gold(x - 1, y, "New Hero", JustCentre, w);

	DrawSelector16(x, selectorTop + 3, w, 32, 26);

	RenderStats("-", "-", "-", "-", "-");

	PrintText30Gold(279, 429, "OK");
	PrintText30Gold(378, 429, "Delete");
	PrintText30Gold(501, 429, "Cancel");
}

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

void DrawPreGameOptions(int ShowClasses)
{
	LoadTitelArt("ui_art\\selhero.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	RenderDiabloLogoSm();

	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, 0, pPcxHeroImage);

	char *GameOptions[2] = { "New Game", "Load Game" };

	// this should not be hard coded.
	int x = 280;
	int y = 256;

	if (ShowClasses == 1) {
		for (int i = 0; i < 2; i++) {
			y += 40;
			PrintText16Silver(x, y, GameOptions[i]);
		}
	}
}

void DrawPreGameDifficultySelection(int HeroClass, int ShowClasses)
{
	LoadTitelArt("ui_art\\selhero.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	RenderDiabloLogoSm();

	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, 0, pPcxHeroImage);

	RenderStats("1", "30", "10", "20", "25");

	char *GameOptions[3] = { "Normal", "Nightmare", "Hell" };

	// this should not be hard coded.
	int x = 280;
	int y = 256;
	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, HeroClass, pPcxHeroImage);

	if (ShowClasses == 1) {
		for (int i = 0; i < 3; i++) {
			y += 40;
			PrintText16Silver(x, y, GameOptions[i]);
		}
	}
}

void RenderDefaultStats(int HeroClass)
{
	switch (HeroClass) {
	case UI_WARRIOR:
		RenderStats("1", "30", "10", "20", "25");
		break;
	case UI_ROGUE:
		RenderStats("1", "20", "15", "30", "20");
		break;
	case UI_SORCERER:
		RenderStats("1", "15", "35", "15", "20");
		break;
	}
}

void SetHeroStats(_uiheroinfo *a1)
{
	memcpy(&heroarray[TotalPlayers], a1, sizeof(_uiheroinfo));
}

void LoadHeroStats()
{
	pfile_ui_set_hero_infos(SetHeroStats);
}

// Have this load the function above and then render it in the main menu.
// Cnacel box is also needed.
void CreateHeroMenu()
{
	LoadTitelArt("ui_art\\selhero.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	RenderDiabloLogoSm();

	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, SelectedItem - 1, pPcxHeroImage);

	PrintText30Silver(-1, 161, "New Single Player Hero", JustCentre);

	int w = 369;
	int x = 241;
	int y = 285;

	PrintText30Silver(x - 1, 211, "Choose Class", JustCentre, w);

	RenderDefaultStats(SelectedItem - 1);

	char *heroclasses[3] = { "Warrior", "Rogue", "Sorcerer" };

	int selectorTop = y;

	for (int i = 0; i < 3; i++) {
		if (i > 1) {
			y += 1; // "Rouge" and "Sorcerer" has a smaller gap then other items
		}
		PrintText24Gold(x - 1, y, heroclasses[i], JustCentre, w);
		y += 33;
	}

	if (SelectedItem > 1) {
		selectorTop += 1; // "Rouge" and "Sorcerer" has a smaller gap then other items
	}

	DrawSelector(x, selectorTop - 2, w, 39, 26);

	PrintText30Gold(329, 429, "OK");
	PrintText30Gold(451, 429, "Cancel");
}

void RenderUndecidedHeroName(int HeroClass)
{
	LoadTitelArt("ui_art\\selhero.pcx");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	RenderDiabloLogoSm();

	DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, HeroClass, pPcxHeroImage);

	PrintText30Silver(-1, 161, "New Single Player Hero", JustCentre);

	int w = 369;
	int x = 241;
	int y = 318;

	PrintText30Silver(x - 1, 211, "Enter Name", JustCentre, w);

	RenderDefaultStats(HeroClass);

	DrawSelector(x, y - 2, w, 39, 26);
	PrintText24Gold(x + 67, y, (char *)HeroUndecidedName); // todo add blinking "|"

	PrintText30Gold(329, 429, "OK");
	PrintText30Gold(451, 429, "Cancel");
}
