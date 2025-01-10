#pragma once

#include "graphics.h"
#include "streetNet.h"
#include <vector>
#include <thread>

static std::string currentVersion = "0.3.69MP";



static std::string pathSt = "toronto_canada.streets.bin";
static std::string pathOSM = "toronto_canada.osm.bin";


//Global variable for the event loop 
static std::string cityName = "Toronto";
static std::pair<double, double> PinLocation = {0, 0};
static std::pair<unsigned, unsigned> compare = {0, 0};
static unsigned intersecSnack = 0;
static unsigned POISnack = 0;
static std::pair<double, double> startLocation = {0, 0};
static std::pair<double, double> endLocation = {0, 0};

static std::string keyboardInput;
static std::string string2show;
static std::string suggest = "";
static std::string suggestOffset = "";

static std::string secondInput = "";
static std::string secondString2show = "";
static std::string secondSuggest = "";
static std::string secondSuggestOffset = "";

static bool quitFlag = false;
static bool zoom_fitFlag = false;
static bool postscriptFlag = false;

static bool mouseOverClearButton = false;
static bool mouseOverNegButton = false;
static bool mouseOverSearchBar = false;
//static bool mouseOverNegMenu = false;
static bool searchMod = false;
static bool searchNevMod = false;
static bool searchNevInputing = false;
static bool mouseOverSearchButtom = false;
static bool mouseOverNevSearch = false;
static bool mouseOverMenuButtom = false;
static bool mouseOverZoomBar = false;
static bool mouseOverExplore = false;
static bool mouseOverTransit = false;
static bool mouseOverSystem = false;
static bool mouseOverAbout = false;
static int exploreItem = 0;
static int exploreItemON = 0;
static bool debugMode = false;
static std::pair<int, int> rightClickMenu(0, 0);//the location where right clicked, (0,0) its not
static int mouseOverRightClickMenu = 0; // y position on the mouse in a right click menu
static bool menuOut = false;
static bool exploreMenu = false;
static bool transitMenu = false;
static bool systemMenu = false;
static bool aboutMenu = false;
//static bool NegMenu = false;
static bool bottomMenuShadow = false;

static bool findPath = false;
static bool demoing = false;
static bool AstartON = false;
static bool nevBar = false;
static std::vector<unsigned> selectedSeg;
static double truncost = 15;

static int scrollOffset = 0;
static int scroll = 0; //0 = not scroll, 1 = scroll forward, -1 = scroll back

//this return a value of current map's range
//usefull for set_visible world
t_bound_box getMapRange();

//this resets all globle variable to default
void resetGloble();

//we don't use this 
void act_on_new_button_func(void (*drawscreen_ptr) (void));

//handeling general mouse click
void act_on_button_press(float x, float y, t_event_buttonPressed event);

//handeling general mouse movement
void act_on_mouse_move(float x, float y);

//handeling keyboard input
void act_on_key_press(char c, int keysym);

void delay(long milliseconds);

void drawSearchBoxShadow();

void drawSearchBox();

void drawNevSearchBoxShadow();

void drawNevSearchBox();

void drawPinAnimate();

//set a pin to location from Pinlocation
void drawPin();

void drawStart();

void drawEnd();

//find closest POI, set pin on it and show info through snack bar
void press_on_POI(double x, double y);

//show POI info on a snackar
void POI2snackbar();

void press_on_intersection(double x, double y);

void locate_intersection(unsigned interID);

void intersec2snackbar();

//here is some general perporse Snack bar action
//Snack Bar Show 2/3/4/5 line of massage
void showSnackBar(std::string primery, std::string secondary, 
                std::string tertiary, std::string quaternary, std::string quinary);
void showSnackBar(std::string primery, std::string secondary, 
                    std::string tertiary, std::string quaternary);
void showSnackBar(std::string primery, std::string secondary, std::string tertiary);
void showSnackBar(std::string primery, std::string secondary);
//here is Snack Bar for path infomation
void path2snackBar();

//system level massage shown through here
//NOTE: a duration of 9999 is spacial
//9999 will cause toast not disappare until the next screen re-fresh
void make_a_toast(std::string toast, double duration = 2000);

void drawScaleBar(); 

void drawPOIMenu();

void drawZoomButtom();
void drawNavigationMenu();

//this draw right click menu on the creen,
//redraw = false if it is the first time its been draw in one click
//redraw = true shadow will not be draw
void drawRightClickMenu(bool redraw);

//handleing right click menu items action
void drawRightClickItem(std::string name, unsigned idx);

void SysMenuAnimFrame(int width);

void SysMenuAnimationOut();

void SysMenuAnimationIn();

void drawSysMenu(bool redraw, bool redrawSub);

void drawExploreMenu(bool redraw);

void drawTramsitMenu(bool redraw);

void drawSystemMenu(bool redraw);

void drawAboutMenu(bool redraw);

void drawExploreItem(std::string name, int y);

void act_on_exploreItem_press(bool& POIflag, int y);

void drawNevBar();

void drawDirectionNode(unsigned index, trunInfo& Info, unsigned inde);

void showDotPath();

void formEdgePath();

void showEdgePath();

void showRoute();

void drawCityName();

//this provide a smooth transition from one location to another
void motionAnimation(t_bound_box start, t_bound_box end, unsigned duration=2000);

void drawStatic();

void drawAll();

void redrawAll();

//welcom screen
void LoadingCity();

void drawMap();

//this will check all system flags and act on it when called 
void systemChecker();

void debugInfo();

//this handels text input and its actions
void textParser();

void showNavigation(unsigned ID1,unsigned ID2, unsigned ID3);

