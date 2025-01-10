#include "mappinglib.h"
#include "graphics.h"
#include "streetNet.h"
#include "m1.h"
#include "debug.h"
#include "StreetsDatabaseAPI.h"
#include <X11/keysym.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>
#include <sstream>

//#define drawperf


t_bound_box getMapRange() {
    double bottom = streetNet::getInstance()->getRange(1,0).first;
    double left = streetNet::getInstance()->getRange(1,0).second;
    double top = streetNet::getInstance()->getRange(0,1).first;
    double right = streetNet::getInstance()->getRange(0,1).second;
    double aveLat = streetNet::getInstance()->getInterAveLat();
    t_bound_box Range(left * DEG_TO_RAD * aveLat, bottom * DEG_TO_RAD,
                   right * DEG_TO_RAD * aveLat, top * DEG_TO_RAD);
    return Range;
}

void resetGloble() {
    PinLocation = {0, 0};
    compare = {0, 0};
    intersecSnack = 0;
    POISnack = 0;

    keyboardInput = "";
    string2show = "";
    suggest = "";
    suggestOffset = "";

    mouseOverSearchBar = false;
    searchMod = false;
    mouseOverSearchButtom = false;
    mouseOverMenuButtom = false;
    mouseOverZoomBar = false;
    mouseOverExplore = false;
    mouseOverTransit = false;
    mouseOverClearButton = false;
    mouseOverNegButton = false;
//    mouseOverNegMenu = false;
//    NegMenu = false;
    exploreItem = 0;
    exploreItemON = 0;
    debugMode = false;
    rightClickMenu.first = 0;
    rightClickMenu.second = 0;
    menuOut = false;
    exploreMenu = false;
    transitMenu = false;
}


void delay(long milliseconds) {
    std::chrono::milliseconds duration(milliseconds);
    std::this_thread::sleep_for(duration);
}

//it's part of example, we don't realy use it
void act_on_new_button_func(void (*drawscreen_ptr) (void)) {

}


void act_on_button_press(float x, float y, t_event_buttonPressed event) {

    /* Called whenever event_loop gets a button press in the graphics *
     * area.  Allows the user to do whatever he/she wants with button *
     * clicks.                                                        */
    
    //Note the that the default x, y is in world coord, which is very nice
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    int scrnX = xworld_to_scrn(x); //here is the screen position
    int scrnY = yworld_to_scrn(y);
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();

    //std::cout << "User clicked a mouse button at coordinates ("
    //        << x << "," << y << ")";
//    if (event.shift_pressed || event.ctrl_pressed) {
////        std::cout << " with ";
//        if (event.shift_pressed) {
////            std::cout << "shift ";
//            if (event.ctrl_pressed)
////                std::cout << "and ";
//        }
//        if (event.ctrl_pressed)
////            std::cout << "control ";
////        std::cout << "pressed.";
//    }
////    std::cout << std::endl;
//    
    
    if (event.button == Button3) {
        rightClickMenu.first = scrnX;
        rightClickMenu.second = scrnY;
    }
    else if (mouseOverRightClickMenu == 0){
        rightClickMenu.first = 0;
        rightClickMenu.second = 0;
    }
    
    
    if (event.button == Button4) {
        scroll = 1;
        drawMap();
    }
    else if (event.button == Button5) {
        scroll = -1;
        drawMap();
    }
    else {
        scroll = 0;
    }
    
    //click on menu bar
    if (scrnX >= 20 && scrnX <= 60 && scrnY >= 20 && scrnY <= 60) {
        if (menuOut) {
            menuOut = false;
            SysMenuAnimationIn();
        }
        else {
            menuOut = true;
            SysMenuAnimationOut();
        }
        drawMap();
    }
    if (scrnX > 370 && menuOut) {
        menuOut = false;
        SysMenuAnimationIn();
        exploreMenu = false;
        transitMenu = false;
        drawMap();
    }
    
    //click on explore items
    if (scrnX >= 370 && scrnX <= 600 && exploreMenu) {
        exploreItem = scrnY;
        exploreItemON = scrnY;
        act_on_exploreItem_press(streetNet::getInstance()->restarantON, 30);
        act_on_exploreItem_press(streetNet::getInstance()->funPlaceON, 70);
        act_on_exploreItem_press(streetNet::getInstance()->icecreamON, 110);
        act_on_exploreItem_press(streetNet::getInstance()->cafeON, 150);
        act_on_exploreItem_press(streetNet::getInstance()->psychicON, 190);
        act_on_exploreItem_press(streetNet::getInstance()->hospitalON, 230);
        act_on_exploreItem_press(streetNet::getInstance()->telephoneON, 270);
        act_on_exploreItem_press(streetNet::getInstance()->condoON, 310);
        act_on_exploreItem_press(streetNet::getInstance()->spaON, 350);
        act_on_exploreItem_press(streetNet::getInstance()->pharmacyON, 390);
        act_on_exploreItem_press(streetNet::getInstance()->parkingON, 430);
        act_on_exploreItem_press(streetNet::getInstance()->bankON, 470);
        act_on_exploreItem_press(streetNet::getInstance()->collegeON, 510);
        act_on_exploreItem_press(streetNet::getInstance()->childcareON, 550);
        act_on_exploreItem_press(streetNet::getInstance()->libraryON, 590);
        act_on_exploreItem_press(streetNet::getInstance()->pubON, 630);
        act_on_exploreItem_press(streetNet::getInstance()->labON, 670);
        act_on_exploreItem_press(streetNet::getInstance()->schoolON, 710);
        act_on_exploreItem_press(streetNet::getInstance()->lawyerON, 750);
        act_on_exploreItem_press(streetNet::getInstance()->cinemaON, 790);
        act_on_exploreItem_press(streetNet::getInstance()->atmON, 830);
        act_on_exploreItem_press(streetNet::getInstance()->policeON, 870);
        act_on_exploreItem_press(streetNet::getInstance()->fuelON, 910);
        
        drawMap();
    }
    if (scrnX >= 370 && scrnX <= 600 && transitMenu) {
        exploreItem = scrnY;
        exploreItemON = scrnY;
        act_on_exploreItem_press(streetNet::getInstance()->subwayON, 30);
        act_on_exploreItem_press(streetNet::getInstance()->stCartON, 70);
        act_on_exploreItem_press(streetNet::getInstance()->busON, 110);
//        act_on_exploreItem_press(streetNet::getInstance()->cafeON, 150);
        
        drawMap();
    }
    
        if (scrnX >= 370 && scrnX <= 600 && systemMenu) {
        exploreItem = scrnY;
        exploreItemON = scrnY;
        act_on_exploreItem_press(quitFlag, 30);
        act_on_exploreItem_press(zoom_fitFlag, 70);
        act_on_exploreItem_press(postscriptFlag, 110);
//        act_on_exploreItem_press(streetNet::getInstance()->cafeON, 150);
        
        systemChecker();
    }
    
    if (!(scrnX >= 370 && scrnX <= 600) && exploreItem != 0) {
        exploreItem = 0;
        drawMap();
    }
    
    //click on search bar
    if (scrnX >= 270 && scrnX <= 310 && scrnY >= 20 && scrnY <= 60) {
        textParser();
        keyboardInput = "";
    }
    
    //click on search button
    if (scrnX >= 20 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) {
        searchMod = true;
        searchNevInputing = false;
        drawMap();
    }
    else if (searchMod || searchNevInputing) {
        searchMod = false;
        searchNevInputing = false;
        drawMap();
    }
    
    //click on Nev search button
    if (scrnX >= 320 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) {
        searchNevMod = true;
        drawMap();
        return;
    }
    if (!(scrnX >= 20 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) 
            && !(scrnX >= 20 && scrnX <= 350 && scrnY >= 80 && scrnY <= 120)
            && searchNevMod) {
        searchNevMod = false;
        drawMap();
        return;
    }
    if ((scrnX >= 20 && scrnX <= 350 && scrnY >= 80 && scrnY <= 120) && searchNevMod) {
        searchNevInputing = true;
        searchMod = false;
        drawMap();
        return;
    }
    
    //click on zoom in
    if (scrnX >= (windowWidth - 50) && scrnX <= (windowWidth - 20) 
            && scrnY >= (windowHeight - 80) && scrnY <= (windowHeight - 50)) {
        zoom_in(drawMap);
        return;
    }
    if (scrnX >= (windowWidth - 50) && scrnX <= (windowWidth - 20) 
            && scrnY >= (windowHeight - 50) && scrnY <= (windowHeight - 20)) {
        zoom_out(drawMap);
        return;
    }
    
    //click on right click menu
    if (mouseOverRightClickMenu != 0) { 
        //item #0: set start
        int unperBound = rightClickMenu.second + 3;
        int lowBound = rightClickMenu.second + 3 + 25;
        int rightBound = rightClickMenu.first + 150;     
        if (scrnY > unperBound && scrnY < lowBound
                && scrnX > rightClickMenu.first && scrnX < rightBound) {
            startLocation.first = PinLocation.first;
            startLocation.second = PinLocation.second;
            PinLocation = {0, 0}; //reset pin location
            rightClickMenu.first = 0;
            rightClickMenu.second = 0; //close menu after click
//            formEdgePath();
            drawMap();
            return;
        }
        
        //item #1: set end
        unperBound = rightClickMenu.second + 3 + 25;
        lowBound = rightClickMenu.second + 3 + 25 + 25; 
        if (scrnY > unperBound && scrnY < lowBound
                && scrnX > rightClickMenu.first && scrnX < rightBound) {
            endLocation.first = PinLocation.first;
            endLocation.second = PinLocation.second;
            PinLocation = {0, 0}; //reset pin location
            rightClickMenu.first = 0;
            rightClickMenu.second = 0; //close menu after click
            drawMap();
            formEdgePath();
            findPath = true;           //engage path finding after end is set
            drawMap();
            return;
        }
        
        //item #6
        unperBound = rightClickMenu.second + 3 + (25 * 6);
        lowBound = rightClickMenu.second + 3 + 25 + (25 * 6); 
        if (scrnY > unperBound && scrnY < lowBound
                && scrnX > rightClickMenu.first && scrnX < rightBound && debugMode) {
            LatLon curserLocation(PinLocation.second / DEG_TO_RAD, PinLocation.first /(DEG_TO_RAD * cosAveLat));
            unsigned newinter = find_closest_intersection(curserLocation);
            selectedInter.push_back(newinter);
            rightClickMenu.first = 0;
            rightClickMenu.second = 0; //close menu after click
            drawMap();
            return;
        }
    return;
    }
    
    //click on clear button on snack bar
    if((scrnX >=  (midScreen - 200) && scrnX <= (midScreen - 145) && (scrnY >= bottonScreen - 120) && scrnY <= (bottonScreen - 30))&&mouseOverClearButton){
        selectedInter.clear();
        startLocation = {0, 0};
        endLocation = {0, 0};
        findPath = false;
        drawMap();
        return;
    }
    
    //click on snack bar nev button
    if (scrnX >=  (midScreen +145) && scrnX <= (midScreen +200) 
            && (scrnY >= bottonScreen - 120) 
            && scrnY <= (bottonScreen - 30) && findPath&&mouseOverNegButton && !nevBar) {
        nevBar = true;
        drawMap();
        return;
    }
    if ((scrnX >=  (midScreen +145) && scrnX <= (midScreen +200) 
            && (scrnY >= bottonScreen - 120) 
            && scrnY <= (bottonScreen - 30)) && nevBar) {
        nevBar = false;
        drawMap();
        return;
    }
/*    
    if((scrnX >=  (midScreen +145) && scrnX <= (midScreen +200) && (scrnY >= bottonScreen - 120) && scrnY <= (bottonScreen - 30))&&mouseOverNegButton){
        NegMenu = true;
        drawMap();
     return;
    }
        
    if((scrnX <= windowWidth-380 ||scrnY <=windowHeight-730 ||scrnX >= windowWidth-20||scrnY >= windowHeight-100)&&NegMenu){
        NegMenu = false;
        
        drawMap();
        return;
    }
   
*/    
    //click on intersection
    if (get_visible_world().get_height() > 0.00003 && !searchMod) {  
        press_on_intersection(x, y);
        drawMap();
        return;
    }
    //click on POI
    if (get_visible_world().get_height() <= 0.00003 && !searchMod) {
        press_on_POI(x, y);
        drawMap();
        return;
    }
    drawMap();
}


void act_on_mouse_move(float x, float y) {
    // function to handle mouse move event, the current mouse position in the current world coordinate
    // system (as defined in your call to init_world) is returned
    
    int scrnX = xworld_to_scrn(x);
    int scrnY = yworld_to_scrn(y);
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    if (debugMode) {
        set_coordinate_system(GL_SCREEN);
        setcolor(RED);
        settextattrs(8, 0);        
        drawtext(windowWidth -100, 55, std::to_string(scrnX) + "," + std::to_string(scrnY));
//        debugInfo();
        set_coordinate_system(GL_WORLD);
    }
    
    //here we check weather the cursor is over the search bar or not
    if (scrnX >= 20 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) {
        mouseOverSearchBar = true;
        if (scrnX >= 270 && scrnX <= 310 && scrnY >= 20 && scrnY <= 60) {
            mouseOverSearchButtom = true;
        }
        if (scrnX >= 320 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) {
            mouseOverNevSearch = true;
        }
        if (scrnX >= 20 && scrnX <= 60 && scrnY >= 20 && scrnY <= 60) {
            mouseOverMenuButtom = true;
        }
        drawMap();
    }
    else if (mouseOverSearchBar && !searchMod) {
        mouseOverSearchBar = false;
        drawMap();
    }
    
    if (!(scrnX >= 270 && scrnX <= 310 && scrnY >= 20 && scrnY <= 60) && mouseOverSearchButtom) {
        mouseOverSearchButtom = false;
        drawMap();
    }
    
    if (!(scrnX >= 320 && scrnX <= 350 && scrnY >= 20 && scrnY <= 60) && mouseOverNevSearch) {
        mouseOverNevSearch = false;
        drawMap();
    }
    
    if (!(scrnX >= 20 && scrnX <= 60 && scrnY >= 20 && scrnY <= 60) && mouseOverMenuButtom) {
        mouseOverMenuButtom = false;
        drawMap();
    }
    
    //zoom button
    if (scrnX >= (windowWidth - 50) && scrnX <= (windowWidth - 20) 
            && scrnY >= (windowHeight - 80) && scrnY <= (windowHeight - 20)) {
        mouseOverZoomBar = true;
        drawMap();
    }
    if (!(scrnX >= (windowWidth - 50) && scrnX <= (windowWidth - 20) 
            && scrnY >= (windowHeight - 80) && scrnY <= (windowHeight - 20))
            && mouseOverZoomBar) {
        mouseOverZoomBar = false;
        drawMap();
    }
    
    //explore menu
    if (scrnX <= 370 && scrnY >= 157 && scrnY <= 241 && menuOut && !mouseOverExplore) {
        mouseOverExplore = true;
        transitMenu = false;
        exploreMenu = true;
        systemMenu = false;
        aboutMenu = false;
        drawSysMenu(true, false);
    }
    if (!(scrnX <= 370 && scrnY >= 157 && scrnY <= 241) && mouseOverExplore && !exploreMenu) {
        mouseOverExplore = false;
        drawSysMenu(true, false);
    }
    
    //explore item
    if (scrnX >= 370 && scrnX <= 600 && (exploreMenu || transitMenu || systemMenu || aboutMenu)) {
        exploreItem = scrnY;
        drawSysMenu(true, true);
    }
    if (!(scrnX >= 370 && scrnX <= 600) && exploreItem != 0) {
        exploreItem = 0;
        drawSysMenu(true, true);
    }
    
    //Public Transit
    if (scrnX <= 370 && scrnY >= 242 && scrnY <= 326 && menuOut && !mouseOverTransit) {
        mouseOverTransit = true;
        transitMenu = true;
        exploreMenu = false;
        systemMenu = false;
        aboutMenu = false;
        drawSysMenu(true, false);
    }
    if (!(scrnX <= 370 && scrnY >= 242 && scrnY <= 326) && mouseOverTransit && !transitMenu) {
        mouseOverTransit = false;
        drawSysMenu(true, false);
    }
    
    //System
    if (scrnX <= 370 && scrnY >= 327 && scrnY <= 411 && menuOut && !mouseOverSystem) {
        mouseOverSystem = true;
        systemMenu = true;
        transitMenu = false;
        exploreMenu = false;
        aboutMenu = false;
        drawSysMenu(true, false);
    }
    if (!(scrnX <= 370 && scrnY >= 327 && scrnY <= 411) && mouseOverSystem && !systemMenu) {
        mouseOverSystem = false;
        drawSysMenu(true, false);
    }
    
    //About
    if (scrnX <= 370 && scrnY >= 412 && scrnY <= 496 && menuOut && !mouseOverAbout) {
        mouseOverAbout = true;
        aboutMenu = true;
        systemMenu = false;
        transitMenu = false;
        exploreMenu = false;
        drawSysMenu(true, false);
    }
    if (!(scrnX <= 370 && scrnY >= 412 && scrnY <= 496) && mouseOverAbout && !systemMenu) {
        mouseOverAbout = false;
        drawSysMenu(true, false);
    }
    
    //clear button on snack bar
    if(scrnX >=  (midScreen - 200) && scrnX <= (midScreen - 145) 
            && (scrnY >= bottonScreen - 120) 
            && scrnY <= (bottonScreen - 30) && findPath){      
        mouseOverClearButton = true;
        drawMap();
    }
    if((scrnX <=  (midScreen - 200) || scrnX >= (midScreen - 145) 
            || (scrnY <= bottonScreen - 120) || scrnY >= (bottonScreen - 30))
            && mouseOverClearButton){
        mouseOverClearButton = false;
        drawMap();
    }
        
        //navigation button on snack bar
    if(scrnX >=  (midScreen +145) && scrnX <= (midScreen +200) 
            && (scrnY >= bottonScreen - 120) 
            && scrnY <= (bottonScreen - 30) && findPath && !mouseOverNegButton){      
        mouseOverNegButton = true;
        drawMap();
    }
    if(!(scrnX >=  (midScreen +145) && scrnX <= (midScreen +200) 
            && (scrnY >= bottonScreen - 120) 
            && scrnY <= (bottonScreen - 30) && findPath)
            && mouseOverNegButton){
        mouseOverNegButton = false;
        drawMap();
    }
/*    
    if((scrnX >=  (windowWidth-380)&& scrnY >=(windowHeight-730) &&scrnX <= (windowWidth-20) && scrnY <= (windowHeight-100))&&NegMenu){
        mouseOverNegMenu = true;
        drawMap();
    }
    
    if((scrnX <=  (windowWidth-380)|| scrnY <=(windowHeight-730) ||scrnX >= (windowWidth-20) || scrnY >= (windowHeight-100))&&NegMenu){
        mouseOverNegMenu = false;
        drawMap();
    }
    
*/

        
    //right click menu items
    if (scrnX >= rightClickMenu.first && scrnX <= rightClickMenu.first + 150 
            && scrnY >= rightClickMenu.second && scrnY <= rightClickMenu.second + 260
            && rightClickMenu.first != 0 && rightClickMenu.second != 0) {
        mouseOverRightClickMenu = scrnY;
        drawRightClickMenu(true);
    }
    if (!(scrnX >= rightClickMenu.first && scrnX <= rightClickMenu.first + 150 
            && scrnY >= rightClickMenu.second && scrnY <= rightClickMenu.second + 260
            && rightClickMenu.first != 0 && rightClickMenu.second != 0)
            && mouseOverRightClickMenu != 0) {
        mouseOverRightClickMenu = 0;
        drawMap();
    }
    
    
    if (scrnX <= 370 && !notzoom && nevBar) notzoom = true;
    if (scrnX >= 370 && notzoom && nevBar) notzoom = false;


    //here is some debugging code
    if (debugMode) debugInfo();
}




void act_on_key_press(char c, int keysym) {
    // function to handle keyboard press event, the ASCII character is returned
    // along with an extended code (keysym) on X11 to represent non-ASCII
    // characters like the arrow keys.
    
    int cursorOffset = 0;
    int secondCursorOffset = 0;
//    std::cout << "Key press: char is " << c << std::endl;
    if ((!searchMod) && (!searchNevInputing)) {
        switch (keysym) {
            case XK_Left:
//                std::cout << "Left Arrow" << std::endl;
                translate_left(drawMap);
                break;
            case XK_Right:
//                std::cout << "Right Arrow" << std::endl;
                translate_right(drawMap);
                break;
            case XK_Up:
//                std::cout << "Up Arrow" << std::endl;
                translate_up(drawMap);
                break;
            case XK_Down:
//                std::cout << "Down Arrow" << std::endl;
                translate_down(drawMap);
                break;
            default:
//                std::cout << "keysym (extended code) is " << keysym << std::endl;
                break;
        }
    }
    
    else if (searchMod) {
    #ifdef X11 // Extended keyboard codes only supported for X11 for now
        switch (keysym) {
            case XK_Left:
//                std::cout << "Left Arrow" << std::endl;
                translate_left(drawMap);
                break;
            case XK_Right:
//                std::cout << "Right Arrow" << std::endl;
                translate_right(drawMap);
                break;
            case XK_Up:
//                std::cout << "Up Arrow" << std::endl;
                translate_up(drawMap);
                break;
            case XK_Down:
//                std::cout << "Down Arrow" << std::endl;
                translate_down(drawMap);
                break;
            case 65288: //Back space
                keyboardInput = keyboardInput.substr(0, keyboardInput.size()-1);
                string2show = string2show.substr(0, string2show.size()-1);
                cursorOffset = get_text_length(string2show);
                break;
            case 65293: //Enter
                textParser();
                keyboardInput = "";
                string2show = "";
                break;
            case 65509: // CAPlock
                break;
            case 65505:
                break;
            case 65507:
                keyboardInput = suggest;
                string2show = suggest;
                break;
            case 32:   //space
                keyboardInput = keyboardInput + c;
                string2show = string2show + c;
                cursorOffset = get_text_length(string2show);
                break;
            default:
//                std::cout << "keysym (extended code) is " << keysym << std::endl;
                //let's make it draw
                keyboardInput = keyboardInput + c;
                string2show = string2show + c;
                cursorOffset = get_text_length(string2show);
                break;
        }
    #endif
        int i = 1;

        while (cursorOffset > 190) {
            int len = keyboardInput.length();
            string2show = keyboardInput.substr(i,len-1);
            suggest.erase(0,1);
            cursorOffset = get_text_length(string2show);
            i ++;
        }

        set_drawing_buffer(ON_SCREEN);
        drawSearchBox();
        set_coordinate_system(GL_SCREEN);
        //here we handle all the text input

        setcolor(150, 150, 150);
        settextattrs(14, 0);
        drawtext_left(70, 40, string2show, 99999, 99999);
        flushinput();


        //here we draw a cursor

        set_draw_mode(DRAW_NORMAL); 
        setlinestyle(SOLID, ROUND);
        setlinewidth(1);
        setcolor(210, 210, 210); 
        drawline(72 + cursorOffset, 30, 72 + cursorOffset, 50);

        set_coordinate_system(GL_WORLD);
        set_drawing_buffer(OFF_SCREEN);
        
        //the following duel with auto suggest
        std::stringstream inputvalue(keyboardInput);
    
        std::string parsed1, parsed2;
    
        std::getline(inputvalue, parsed1, ',');
        std::getline(inputvalue, parsed2);
        
        if (c == ',') {
            int len = keyboardInput.length();
            suggestOffset = parsed1 + ",";
        }
        if (suggestOffset.compare("") == 0) {
            suggest = streetNet::getInstance()->autoComplete(keyboardInput);
        }
        else {
            suggest = suggestOffset + streetNet::getInstance()->autoComplete(parsed2);
        }
    }
    
    else if (searchNevInputing) {
    #ifdef X11 // Extended keyboard codes only supported for X11 for now
        switch (keysym) {
            case XK_Left:
//                std::cout << "Left Arrow" << std::endl;
                translate_left(drawMap);
                break;
            case XK_Right:
//                std::cout << "Right Arrow" << std::endl;
                translate_right(drawMap);
                break;
            case XK_Up:
//                std::cout << "Up Arrow" << std::endl;
                translate_up(drawMap);
                break;
            case XK_Down:
//                std::cout << "Down Arrow" << std::endl;
                translate_down(drawMap);
                break;
            case 65288: //Back space
                secondInput = secondInput.substr(0, secondInput.size()-1);
                secondString2show = secondString2show.substr(0, secondString2show.size()-1);
                secondCursorOffset = get_text_length(secondString2show);
                break;
            case 65293: //Enter
                textParser();
                nevBar = true;
                drawMap();
                break;
            case 65509: // CAPlock
                break;
            case 65505:
                break;
            case 65507: //ctrl to auto complete
                secondInput = secondSuggest;
                secondString2show = secondSuggest;
                break;
            case 32:   //space
                secondInput = secondInput + c;
                secondString2show = secondString2show + c;
                secondCursorOffset = get_text_length(secondString2show);
                break;
            default:
//                std::cout << "keysym (extended code) is " << keysym << std::endl;
                //let's make it draw
                secondInput = secondInput + c;
                secondString2show = secondString2show + c;
                secondCursorOffset = get_text_length(secondString2show);
                break;
        }
    #endif
        int i = 1;

        while (secondCursorOffset > 190) {
            int len = secondInput.length();
            secondString2show = secondInput.substr(i,len-1);
            secondSuggest.erase(0,1);
            secondCursorOffset = get_text_length(secondString2show);
            i ++;
        }

        set_drawing_buffer(ON_SCREEN);
        drawNevSearchBox();
        set_coordinate_system(GL_SCREEN);
        //here we handle all the text input

        setcolor(150, 150, 150);
        settextattrs(14, 0);
        drawtext_left(70, 100, secondString2show, 99999, 99999);
        flushinput();


        //here we draw a cursor

        set_draw_mode(DRAW_NORMAL); 
        setlinestyle(SOLID, ROUND);
        setlinewidth(1);
        setcolor(210, 210, 210); 
        drawline(72 + secondCursorOffset, 90, 72 + secondCursorOffset, 110);

        set_coordinate_system(GL_WORLD);
        set_drawing_buffer(OFF_SCREEN);
        
        //the following duel with auto suggest
        std::stringstream inputvalue(secondInput);
    
        std::string parsed1, parsed2;
    
        std::getline(inputvalue, parsed1, ',');
        std::getline(inputvalue, parsed2);
        
        if (c == ',') {
            int len = secondInput.length();
            secondSuggestOffset = parsed1 + ",";
        }
        if (secondSuggestOffset.compare("") == 0) {
            secondSuggest = streetNet::getInstance()->autoComplete(secondInput);
        }
        else {
            secondSuggest = secondSuggestOffset + streetNet::getInstance()->autoComplete(parsed2);
        }
    }
}


void drawSearchBoxShadow() {
    set_coordinate_system(GL_SCREEN);
    //first draw a shadow
    if (mouseOverSearchBar) {
        for (unsigned i = 0; i < 5; i ++) {
            t_point sbl(20+i, 60+i);
            t_point stl(20+i, 20+i);
            t_point sbr(350+i, 60+i);
            t_point str(350+i, 20+i);
            t_point shadowBox[4] = {sbl, stl, str, sbr};
            setcolor(0, 0, 0, 32);
            fillpoly(shadowBox, 4);
        }
        t_point sbl(20-1, 60+1);
        t_point stl(20-1, 20-1);
        t_point sbr(350+1, 60+1);
        t_point str(350+1, 20-1);
        t_point shadowBox[4] = {sbl, stl, str, sbr};
        setcolor(0, 0, 0, 25);
        fillpoly(shadowBox, 4);
    }
    else {
        for (unsigned i = 0; i < 4; i ++) {
            t_point sbl(20+i, 60+i);
            t_point stl(20+i, 20+i);
            t_point sbr(350+i, 60+i);
            t_point str(350+i, 20+i);
            t_point shadowBox[4] = {sbl, stl, str, sbr};
            setcolor(0, 0, 0, 32);
            fillpoly(shadowBox, 4);
        }
        t_point sbl(20-1, 60+1);
        t_point stl(20-1, 20-1);
        t_point sbr(350+1, 60+1);
        t_point str(350+1, 20-1);
        t_point shadowBox[4] = {sbl, stl, str, sbr};
        setcolor(0, 0, 0, 15);
        fillpoly(shadowBox, 4);
    }
    
    set_coordinate_system(GL_WORLD);
}
 

void drawSearchBox() {
    //draw search box here ++++++++++++++++++
    set_coordinate_system(GL_SCREEN);
    //first draw a shadow
//    for (unsigned i = 0; i < 4; i ++) {
//        t_point sbl(20+i, 60+i);
//        t_point stl(20+i, 20+i);
//        t_point sbr(350+i, 60+i);
//        t_point str(350+i, 20+i);
//        t_point shadowBox[4] = {sbl, stl, str, sbr};
//        setcolor(0, 0, 0, 32);
//        fillpoly(shadowBox, 4);
//    }
//    t_point sbl(20-1, 60+1);
//    t_point stl(20-1, 20-1);
//    t_point sbr(350+1, 60+1);
//    t_point str(350+1, 20-1);
//    t_point shadowBox[4] = {sbl, stl, str, sbr};
//    setcolor(0, 0, 0, 15);
//    fillpoly(shadowBox, 4);
    
    t_point bl(20, 60);
    t_point tl(20, 20);
    t_point br(350, 60);
    t_point tr(350, 20);
    t_point textBox[4] = {bl, tl, tr, br};
    setcolor(WHITE);
    fillpoly(textBox, 4);
    
    //draw direction icon+++++++
    Surface dirctionIcon("/homes/x/xuwenkai/ECE297/mapper/turn-right.png");
    Surface dirctionIconDark("/homes/x/xuwenkai/ECE297/mapper/turn-right-bark.png");
    if (mouseOverNevSearch) {
        draw_surface(dirctionIconDark, 320, 28);
    }
//    else draw_surface(dirctionIcon, 320, 28);
    
    //draw search icon
    Surface searchIcon("/homes/x/xuwenkai/ECE297/mapper/search-interface-symbol.png");
    Surface searchIconBlack("/homes/x/xuwenkai/ECE297/mapper/search-interface-symbol-black.png");
    if (mouseOverSearchButtom) {
        draw_surface(searchIconBlack, 280, 28);
    }
    else draw_surface(searchIcon, 280, 28);
    
    //draw menu icon
    Surface menuIcon("/homes/x/xuwenkai/ECE297/mapper/list.png");
    Surface menuIconBlack("/homes/x/xuwenkai/ECE297/mapper/list-black.png");
    if (mouseOverMenuButtom) {
        draw_surface(menuIconBlack, 28, 28);
    }
    else draw_surface(menuIcon, 28, 28);
    
    //draw a line
    set_draw_mode(DRAW_NORMAL); 
    setlinestyle(SOLID, ROUND);
    setlinewidth(2);
    setcolor(200, 200, 200); 
    drawline(312, 25, 312, 55);
    
    //here we draw a line under text area
    if (searchMod) {
        set_draw_mode(DRAW_NORMAL); 
        setlinestyle(SOLID, ROUND);
        setlinewidth(1);
        setcolor(200, 200, 200); 
        drawline(67, 53, 270, 53);
    }
    
    //here we handle all the text input
    setcolor(150, 150, 150);
    settextattrs(14, 0);
    drawtext_left(70, 40, string2show, 99999, 99999);
    //show suggest input
    if (keyboardInput.length() >= 2) {
        setcolor(220, 220, 220);
        settextattrs(14, 0);
        drawtext_left(70, 40, suggest, 99999, 99999);
    }
    
    
    set_coordinate_system(GL_WORLD); //set it back, ALWAYS
}


void drawNevSearchBoxShadow() {
    set_coordinate_system(GL_SCREEN);
    
    for (unsigned i = 0; i < 4; ++ i) {
        setcolor(0, 0, 0, 32);
        fillrect(20 + i, 80 + i, 350 + i, 120 + i);
    }
    setcolor(0, 0, 0, 15);
    fillrect(20 - 1, 80 - 1, 350 + 1, 120 + 1);
    
    
    
    set_coordinate_system(GL_WORLD);
}


void drawNevSearchBox() {
    set_coordinate_system(GL_SCREEN);
    
    setcolor(WHITE);
    fillrect(20, 80, 350, 120);
    
    Surface endIconGray("/homes/x/xuwenkai/ECE297/mapper/flag-gray.png");
    draw_surface(endIconGray, 28, 88);
    
    set_draw_mode(DRAW_NORMAL); 
    setlinestyle(SOLID, ROUND);
    setlinewidth(1);
    setcolor(200, 200, 200); 
    drawline(67, 113, 330, 113);
    
    //here we handle all the text input
    setcolor(150, 150, 150);
    settextattrs(14, 0);
    drawtext_left(70, 100, secondString2show, 99999, 99999);
    //show suggest input
    if (secondInput.length() >= 2) {
        setcolor(220, 220, 220);
        settextattrs(14, 0);
        drawtext_left(70, 100, secondSuggest, 99999, 99999);
    }
    
    set_coordinate_system(GL_WORLD);
}


void drawPinAnimate() { 
    if (PinLocation .first != 0 && PinLocation .second != 0 
            && PinLocation.first != compare.first && PinLocation.second != compare.second) {
        Surface PinIcon("/homes/x/xuwenkai/ECE297/mapper/pin.png");
        
        //here is some very top coordinate convertion 
        double heightInXY = get_visible_world().get_height();
        double heightInPixel = - get_visible_screen().get_height();

        double pixel_to_XY = heightInXY / heightInPixel;
             
        double offset = 24 * pixel_to_XY;
       
        
        for (unsigned i = 0; i <= 3; i ++) {
            drawStatic();
            draw_surface(PinIcon, PinLocation.first - (offset / 2), 
                    (PinLocation.second + offset) + (0.00009 - i * 0.00003));
//            delay(1);
            copy_off_screen_buffer_to_screen();
        }
      
    }
    compare = PinLocation;
}


void drawPin() {
    if (PinLocation .first != 0 && PinLocation .second != 0) {
        
        Surface PinIcon("/homes/x/xuwenkai/ECE297/mapper/pin.png");
        
        //here is some very top coordinate convertion 
        double heightInXY = get_visible_world().get_height();
        double heightInPixel = - get_visible_screen().get_height();

        double pixel_to_XY = heightInXY / heightInPixel;
             
        double offset = 32 * pixel_to_XY; //PNG size is 32*32 in this case
        draw_surface(PinIcon, PinLocation.first - (offset / 2), PinLocation.second + offset);
        
    }
}


void drawStart() {
    Surface startPin("/homes/x/xuwenkai/ECE297/mapper/placeholder.png");
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = 32 * pixel_to_XY; //PNG size is 32*32 in this case
    
    draw_surface(startPin, startLocation.first - (offset / 2), startLocation.second + offset);
}


void drawEnd() {
    Surface startPin("/homes/x/xuwenkai/ECE297/mapper/flag.png");
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = 32 * pixel_to_XY; //PNG size is 32*32 in this case
    
                                                  //do not question this magic number
    draw_surface(startPin, endLocation.first - (offset * 0.1484375), endLocation.second + offset);
}


void press_on_POI(double x, double y) {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    LatLon curserLocation(y / DEG_TO_RAD, x /(DEG_TO_RAD * cosAveLat));
    //std::cout << x << " , " << y << std::endl;
    //std::cout << curserLocation << std::endl;
    unsigned POIid = find_closest_point_of_interest(curserLocation);
    
    PinLocation.first = streetNet::getInstance()
            ->getPOIByID(POIid).getLocation().lon() * DEG_TO_RAD * cosAveLat;
    PinLocation.second = streetNet::getInstance()
            ->getPOIByID(POIid).getLocation().lat() * DEG_TO_RAD;
    
    POISnack = POIid;
}


void POI2snackbar() {
    std::string POIName = streetNet::getInstance()
            ->getPOIByID(POISnack).get_name();
    
    POI currentPOI = streetNet::getInstance()->getPOIByID(POISnack);

    
    std::string type = currentPOI.get_type();
    std::vector<std::pair<std::string, std::string>> exInfo;
    for (unsigned i = 0; i < getTagCount(streetNet::getInstance()->getOSMNode(currentPOI.getOSMID())); ++ i) {
        std::pair<std::string, std::string> newpair = getTagPair(streetNet::getInstance()->getOSMNode(currentPOI.getOSMID()), i);
        if (newpair.first.compare("name") != 0 && newpair.first.compare("amenity") != 0) {
            exInfo.push_back(newpair);
        }
    }
    
    
    if (debugMode) {
        showSnackBar(POIName, type, std::to_string(POISnack));
    }
    else if (exInfo.size() == 0) {
        showSnackBar(POIName, type);
    }
    else if (exInfo.size() == 1) {
        showSnackBar(POIName, type, exInfo[0].first + ": " + exInfo[0].second);
    }
    else if (exInfo.size() == 2) {
        showSnackBar(POIName, type, exInfo[0].first + ": " + exInfo[0].second,
                                    exInfo[1].first + ": " + exInfo[1].second);
    }
    else if (exInfo.size() >= 3) {
        showSnackBar(POIName, type, exInfo[0].first + ": " + exInfo[0].second,
                                    exInfo[1].first + ": " + exInfo[1].second,
                                    exInfo[2].first + ": " + exInfo[2].second);
    }
    
    copy_off_screen_buffer_to_screen();
    
    POISnack = 0;
}


void press_on_intersection(double x, double y) {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    LatLon curserLocation(y / DEG_TO_RAD, x /(DEG_TO_RAD * cosAveLat));
    //std::cout << x << " , " << y << std::endl;
    //std::cout << curserLocation << std::endl;
    unsigned interID = find_closest_intersection(curserLocation);
    
    PinLocation.first = streetNet::getInstance()
            ->getIntersecByID(interID).get_location().lon() * DEG_TO_RAD * cosAveLat;
    PinLocation.second = streetNet::getInstance()
            ->getIntersecByID(interID).get_location().lat() * DEG_TO_RAD;
    
    std::string interName = streetNet::getInstance()
            ->getIntersecByID(interID).get_name();
    intersecSnack = interID;
    update_message(interName);
}


void locate_intersection(unsigned interID) {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    
    PinLocation.first = streetNet::getInstance()
            ->getIntersecByID(interID).get_location().lon() * DEG_TO_RAD * cosAveLat;
    PinLocation.second = streetNet::getInstance()
            ->getIntersecByID(interID).get_location().lat() * DEG_TO_RAD;
    
    std::string interName = streetNet::getInstance()
            ->getIntersecByID(interID).get_name();
    intersecSnack = interID;
    update_message(interName);
    
    t_bound_box newWindow(PinLocation.first - 0.0001, PinLocation.second - 0.0001
                            , PinLocation.first + 0.0001, PinLocation.second + 0.0001);
    set_visible_world(newWindow);
}


void intersec2snackbar() {
    std::string interName = streetNet::getInstance()
            ->getIntersecByID(intersecSnack).get_name();
    
    LatLon location = streetNet::getInstance()
            ->getIntersecByID(intersecSnack).get_location();
    
    
    std::string Lat = std::to_string(location.lat());
    std::string Lon = std::to_string(location.lon());
    
    std::string latlon = Lat + " , " + Lon;
    
    if (debugMode) {
        showSnackBar(interName, latlon, std::to_string(intersecSnack));
    }
    else showSnackBar(interName, latlon);
    
    copy_off_screen_buffer_to_screen();
    
    intersecSnack = 0;
}


void showSnackBar(std::string primery, std::string secondary, 
                std::string tertiary, std::string quaternary, std::string quinary) {
    set_coordinate_system(GL_SCREEN);
    
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    //Draw shadow, because we can
    for (unsigned i = 0; i < 5; i ++) {
        setcolor(0, 0, 0, 20);
        fillrect(midScreen - 200 + i, bottonScreen - 120 + i, 
                midScreen + 200 + i, bottonScreen - 30 + i);
    }
    for (unsigned j = 0; j < 2; j ++) {
        setcolor(0, 0, 0, 15);
        fillrect(midScreen - 200 - j, bottonScreen - 120 - j, 
                    midScreen + 200 + j, bottonScreen - 30 + j);
    }
    
    setcolor(WHITE);
    fillrect(midScreen - 200, bottonScreen - 120, midScreen + 200, bottonScreen - 30);
    
    //draw text
    setcolor(BLACK);
    settextattrs(12, 0);
    drawtext(midScreen, bottonScreen - 105, primery);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 90, secondary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 75, tertiary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 60, quaternary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 45, quinary);
    
    set_coordinate_system(GL_WORLD);
}


void showSnackBar(std::string primery, std::string secondary, 
                    std::string tertiary, std::string quaternary) {
    set_coordinate_system(GL_SCREEN);
    
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    //Draw shadow, because we can
    for (unsigned i = 0; i < 5; i ++) {
        setcolor(0, 0, 0, 20);
        fillrect(midScreen - 200 + i, bottonScreen - 120 + i, 
                midScreen + 200 + i, bottonScreen - 30 + i);
    }
    for (unsigned j = 0; j < 2; j ++) {
        setcolor(0, 0, 0, 15);
        fillrect(midScreen - 200 - j, bottonScreen - 120 - j, 
                    midScreen + 200 + j, bottonScreen - 30 + j);
    }
    
    setcolor(WHITE);
    fillrect(midScreen - 200, bottonScreen - 120, midScreen + 200, bottonScreen - 30);
    
    //draw text
    setcolor(BLACK);
    settextattrs(12, 0);
    drawtext(midScreen, bottonScreen - 100, primery);
    
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext(midScreen, bottonScreen - 80, secondary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 60, tertiary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 45, quaternary);
    
    set_coordinate_system(GL_WORLD);
}
    
 
void showSnackBar(std::string primery, std::string secondary, std::string tertiary) {
    set_coordinate_system(GL_SCREEN);
    
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    //Draw shadow, because we can
    for (unsigned i = 0; i < 5; i ++) {
        setcolor(0, 0, 0, 20);
        fillrect(midScreen - 200 + i, bottonScreen - 120 + i, 
                midScreen + 200 + i, bottonScreen - 30 + i);
    }
    for (unsigned j = 0; j < 2; j ++) {
        setcolor(0, 0, 0, 15);
        fillrect(midScreen - 200 - j, bottonScreen - 120 - j, 
                    midScreen + 200 + j, bottonScreen - 30 + j);
    }
    
    setcolor(WHITE);
    fillrect(midScreen - 200, bottonScreen - 120, midScreen + 200, bottonScreen - 30);
    
    //draw text
    setcolor(BLACK);
    settextattrs(12, 0);
    drawtext(midScreen, bottonScreen - 100, primery);
    
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext(midScreen, bottonScreen - 75, secondary);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 50, tertiary);
    
    set_coordinate_system(GL_WORLD);
}


void showSnackBar(std::string primery, std::string secondary) {
    set_coordinate_system(GL_SCREEN);
    
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    //Draw shadow, because we can
    for (unsigned i = 0; i < 5; i ++) {
        setcolor(0, 0, 0, 20);
        fillrect(midScreen - 200 + i, bottonScreen - 120 + i, 
                midScreen + 200 + i, bottonScreen - 30 + i);
    }
    for (unsigned j = 0; j < 2; j ++) {
        setcolor(0, 0, 0, 15);
        fillrect(midScreen - 200 - j, bottonScreen - 120 - j, 
                    midScreen + 200 + j, bottonScreen - 30 + j);
    }
    
    setcolor(WHITE);
    fillrect(midScreen - 200, bottonScreen - 120, midScreen + 200, bottonScreen - 30);
    
    //draw text
    setcolor(BLACK);
    settextattrs(12, 0);
    drawtext(midScreen, bottonScreen - 90, primery);
    
    setcolor(LIGHTGREY);
    settextattrs(10, 0);
    drawtext(midScreen, bottonScreen - 60, secondary);
    
    
    set_coordinate_system(GL_WORLD);
}


void path2snackBar() {
    set_coordinate_system(GL_SCREEN);
    
    int midScreen = get_visible_screen().get_width() / 2;
    int bottonScreen = get_visible_screen().bottom();
    
    //Draw shadow, because we can
    for (unsigned i = 0; i < 5; i ++) {
        setcolor(0, 0, 0, 20);
        fillrect(midScreen - 200 + i, bottonScreen - 120 + i, 
                midScreen + 200 + i, bottonScreen - 30 + i);
    }
    for (unsigned j = 0; j < 2; j ++) {
        setcolor(0, 0, 0, 15);
        fillrect(midScreen - 200 - j, bottonScreen - 120 - j, 
                    midScreen + 200 + j, bottonScreen - 30 + j);
    }
    
    setcolor(WHITE);
    fillrect(midScreen - 200, bottonScreen - 120, midScreen + 200, bottonScreen - 30);
    
    //here we find all the massages
    std::string fromInter;
    std::string toInter;
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    LatLon startLL(startLocation.second / DEG_TO_RAD, startLocation.first /(DEG_TO_RAD * cosAveLat));
    LatLon endLL(endLocation.second / DEG_TO_RAD, endLocation.first /(DEG_TO_RAD * cosAveLat));
    unsigned start = find_closest_intersection(startLL);
    unsigned end = find_closest_intersection(endLL);
    fromInter = streetNet::getInstance()->getIntersecByID(start).get_name();
    toInter = streetNet::getInstance()->getIntersecByID(end).get_name();
    
    double time = 0;
    unsigned hours = 0;
    unsigned mins = 0;
    unsigned secs = 0;
    std::string traveltime;
    time = streetNet::getInstance()->pathTravelTime(selectedSeg, truncost);
    if (time < 60) {
        secs = time;
        traveltime = to_string(secs) + "s";
    }
    else if (time >= 60 && time < 3600) {
        mins = time / 60;
        secs = int(time) % 60;
        traveltime = to_string(mins) + "min" + to_string(secs) + "s";
    }
    else {
        hours = time / 3600;
        time = time - hours * 3600;
        mins = time / 60;
        secs = int(time) % 60;
        traveltime = to_string(hours) + "h" + to_string(mins) + "min" + to_string(secs) + "s";
    }
    
    unsigned pathlength = streetNet::getInstance()->pathLength(selectedSeg);
    
    
    //draw From and To
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext_left(midScreen - 140, bottonScreen - 105, "From:", 9999, 9999);
    setcolor(64, 64, 64);
    drawtext_left(midScreen - 100, bottonScreen - 105, fromInter, 9999, 9999);
    
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext_left(midScreen - 140, bottonScreen - 85, "To:", 9999, 9999);
    setcolor(64, 64, 64);
    drawtext_left(midScreen - 100, bottonScreen - 85, toInter, 9999, 9999);
    
    //draw distance
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext_left(midScreen - 140, bottonScreen - 65, "Distance:", 9999, 9999);
    setcolor(64, 64, 64);
    drawtext_left(midScreen - 80, bottonScreen - 65, to_string(pathlength) + "m", 9999, 9999);
    
    //draw travel time
    setcolor(DARKGREY);
    settextattrs(11, 0);
    drawtext_left(midScreen - 140, bottonScreen - 45, "Approximate Time:", 9999, 9999);
    setcolor(64, 64, 64);
    drawtext_left(midScreen - 15, bottonScreen - 45, traveltime, 9999, 9999);
    
    //navigate button
    if(mouseOverNegButton){
        setcolor(189, 156, 209);
        fillrect(midScreen +145, bottonScreen- 120 , midScreen +200, bottonScreen- 30);
    }
    Surface NavButton("/homes/w/weiyuyao/mapper_repo/mapper/turn-right-sign.png");
    draw_surface(NavButton, midScreen + 157, bottonScreen - 88);
    

    //draw clear button    
    if(mouseOverClearButton){
        setcolor(189, 156, 209);
        fillrect(midScreen - 200, bottonScreen- 120 , midScreen - 145, bottonScreen- 30);
    }

    Surface clearButton("/homes/x/xuwenkai/ECE297/mapper/cancel-music.png");
    draw_surface(clearButton, midScreen - 182, bottonScreen - 87);
    
    set_coordinate_system(GL_WORLD);
}


void make_a_toast(std::string toast, double duration) {
    if ((!menuOut) && (!searchNevMod)) { //disable when menus are out
        set_coordinate_system(GL_SCREEN);

        //background
        t_point bl(20, 80);
        t_point tl(20, 120);
        t_point br(350, 80);
        t_point tr(350, 120);
        t_point textBox[4] = {bl, tl, tr, br};
        setcolor(64, 64, 64, 100);
        fillpoly(textBox, 4);

        //text
        setcolor(WHITE);
        settextattrs(12, 0);
        drawtext(185, 100, toast);
        copy_off_screen_buffer_to_screen();

        if (duration == 9999) return;

        delay(duration);

        set_coordinate_system(GL_WORLD);
        drawMap();
    }
}


void drawScaleBar() {
    set_coordinate_system(GL_SCREEN);
    
    
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    double worldHeight = get_visible_world().get_height();
    //112 is the size of the scale, in pixels. shall be approximated to the nearest 10th for prod and made to be of non-static length
    //double scale = worldHeight;
    int scale = 112 * ((worldHeight * EARTH_RADIUS_IN_METERS)/windowHeight);
    
    std::string text = std::to_string(scale) + "m";
    set_draw_mode(DRAW_NORMAL); 
    setlinestyle(SOLID, ROUND);
    setlinewidth(2);
    setcolor(0, 0, 0, 128); 
    drawline(windowWidth - 100, windowHeight - 30, windowWidth -212, windowHeight - 30);
    drawline(windowWidth - 100, windowHeight - 30, windowWidth - 100, windowHeight - 35);
    drawline(windowWidth -212, windowHeight - 30, windowWidth -212, windowHeight - 35);
    drawline(windowWidth -175, windowHeight - 30, windowWidth -175, windowHeight - 35);
    drawline(windowWidth -137, windowHeight - 30, windowWidth -137, windowHeight - 35);
    
    settextattrs(10, 0);
    drawtext(windowWidth -156, windowHeight - 20, text);
    
    set_coordinate_system(GL_WORLD);
}


void drawZoomButtom() {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    //shadow
    setcolor(0, 0, 0, 10);
    fillrect(windowWidth - 50 - 1, windowHeight - 80 - 1, windowWidth - 20 + 1, windowHeight- 20 + 1);
    setcolor(0, 0, 0, 32);
    unsigned shadWidth = 3;
    if (mouseOverZoomBar) shadWidth = 4;
    
    for (unsigned i = 0; i < shadWidth; ++ i) {
        fillrect(windowWidth - 50 + i, windowHeight - 80 + i, windowWidth - 20 + i, windowHeight- 20 + i);
    }
    
    setcolor(WHITE);
    fillrect(windowWidth - 50, windowHeight - 80, windowWidth - 20, windowHeight- 20);
    
    setcolor(128, 128, 128);
    settextattrs(15, 0);
    drawtext(windowWidth-35, windowHeight-65, "+");
    drawtext(windowWidth-35, windowHeight-35, "-");
    
    set_coordinate_system(GL_WORLD);
}

void drawRightClickMenu(bool redraw) {
    set_coordinate_system(GL_SCREEN);
    
    //shadow only draw on the first time
    if (!redraw) {
        setcolor(0, 0, 0, 10);
        fillrect(rightClickMenu.first - 1, rightClickMenu.second - 1, 
                rightClickMenu.first + 150 + 1, rightClickMenu.second + 260 + 1);
        setcolor(0, 0, 0, 32);
        for (unsigned i = 0; i < 4; ++ i) {
            fillrect(rightClickMenu.first + i, rightClickMenu.second + i, 
                rightClickMenu.first + 150 + i, rightClickMenu.second + 260 + i);
        }
    }
    
    setcolor(WHITE);
    fillrect(rightClickMenu.first, rightClickMenu.second, 
            rightClickMenu.first + 150, rightClickMenu.second + 260);
    
    //menu items
    drawRightClickItem("Set as Start", 0);
    drawRightClickItem("Set as End", 1);
    drawRightClickItem("Explore Around", 2);
    drawRightClickItem("Feeling Lucky", 3);
    drawRightClickItem("Let me get Drunk", 4);
    drawRightClickItem("Draw a gnome", 5);
    
    if (debugMode) drawRightClickItem("Select Segment", 6);
    
    copy_off_screen_buffer_to_screen();
    set_coordinate_system(GL_WORLD);
}


void drawRightClickItem(std::string name, unsigned idx) {
    int unperBound = rightClickMenu.second + 3 + (25 * idx);
    int lowBound = rightClickMenu.second + 3 + 25 + (25 * idx);
    int rightBound = rightClickMenu.first + 150;
    //hight light
    if (mouseOverRightClickMenu >= unperBound && mouseOverRightClickMenu <= lowBound) {
        setcolor(189, 156, 209);
        fillrect(rightClickMenu.first, unperBound, rightBound, lowBound);
    }
    
    //text
    setcolor(128, 128, 128);
    settextattrs(12, 0);
    drawtext_left(rightClickMenu.first + 5, 
            rightClickMenu.second +15 + (25 * idx), name, 9999, 9999);
}


void drawPOIMenu() {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    //here draw the shadow
    for (unsigned i = 0; i < 2; ++ i) {
        setcolor(0, 0, 0, 16);
        fillarc(windowWidth-50+i, windowHeight-50+i, 30, 0, 360);
    }
    
    setcolor(161, 113, 187);
    fillarc(windowWidth-50, windowHeight-50, 25, 0, 360);
    draw_surface(load_png_from_file("/homes/x/xuwenkai/ECE297/mapper/point-of-interest.png")
                    ,windowWidth-66, windowHeight-65);
    
    set_coordinate_system(GL_WORLD);
}


void SysMenuAnimFrame(int width) {
    set_coordinate_system(GL_SCREEN);
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    double progress = width / 370;  //370 is the target width
    int progressOffset = 370 - width;
    
    //dim the map to a set darkness
    setcolor(0, 0, 0, (int)(64*progress));
    fillrect(get_visible_screen());
    
    //draw shadow here
    setcolor(0, 0, 0, 10);
    for (unsigned i = 0; i < 10; ++ i) {
        fillrect(0, 0, width+1 + i, windowHeight);
    }
    
    //white background
    t_point bl(0, windowHeight);
    t_point tl(0, 0);
    t_point br(width, windowHeight);
    t_point tr(width, 0);
    t_point textBox[4] = {bl, tl, tr, br};
    setcolor(WHITE);
    fillpoly(textBox, 4);
    
    //LOGO area
    setcolor(161, 113, 187);
    fillrect(0, 0, width, 156);
    Surface logo("/homes/x/xuwenkai/ECE297/mapper/logo.png");
    draw_surface(logo, 10 - progressOffset, 105);
    setcolor(WHITE);
    settextattrs(16, 0);
    drawtext(125 - progressOffset, 130, " © MAKS Maps");
    
    //searchBox don't move
    drawSearchBox();
    set_coordinate_system(GL_SCREEN);
    
    //text and icon
    Surface explore("/homes/x/xuwenkai/ECE297/mapper/explore-tool.png");
    draw_surface(explore, 20-progressOffset, 185);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70-progressOffset, 199, "Explore", 9999, 9999);
    
    //Public Transit
    Surface transit("/homes/x/xuwenkai/ECE297/mapper/minibus.png");
    draw_surface(transit, 20-progressOffset, 270);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70-progressOffset, 284, "Public Transit", 9999, 9999);
    
    //System
    Surface system("/homes/x/xuwenkai/ECE297/mapper/settings.png");
    draw_surface(system, 20-progressOffset, 355);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70-progressOffset, 369, "System", 9999, 9999);
    
    //About
    Surface about("/homes/x/xuwenkai/ECE297/mapper/settings.png");
    draw_surface(about, 20-progressOffset, 440);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70-progressOffset, 450, "About MAKS GIS", 9999, 9999);
     
    copy_off_screen_buffer_to_screen();
}


void SysMenuAnimationOut() {
    for (unsigned i = 0; i <= 370; i += 37) {
        SysMenuAnimFrame(i);
        delay(7);
    }
}


void SysMenuAnimationIn() {
    for (unsigned i = 370; i != 0; i -= 37) {
        drawStatic();
        SysMenuAnimFrame(i);
        delay(1);
    }
}


void drawSysMenu(bool redraw, bool redrawSub) {
    if (!menuOut) return;
    set_coordinate_system(GL_SCREEN);
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    if (!redraw) {
        //dim the map
        setcolor(0, 0, 0, 64);
        fillrect(get_visible_screen());
    }
    if (exploreMenu) drawExploreMenu(redrawSub);
    else if (transitMenu) drawTramsitMenu(redrawSub);
    else if (systemMenu) drawSystemMenu(redrawSub);
    else if (aboutMenu) drawAboutMenu(redrawSub);
    set_coordinate_system(GL_SCREEN);
    
    
        //draw shadow
    setcolor(0, 0, 0, 10);
    for (unsigned i = 0; i < 10; ++ i) {
        fillrect(0, 0, 371 + i, windowHeight);
    }
    
    
    t_point bl(0, windowHeight);
    t_point tl(0, 0);
    t_point br(370, windowHeight);
    t_point tr(370, 0);
    
    t_point textBox[4] = {bl, tl, tr, br};
    setcolor(WHITE);
    fillpoly(textBox, 4);
    
    //LOGO area
    setcolor(161, 113, 187);
    fillrect(0, 0, 370, 156);
    Surface logo("/homes/x/xuwenkai/ECE297/mapper/logo.png");
    draw_surface(logo, 10, 105);
    setcolor(WHITE);
    settextattrs(16, 0);
    //Max, Alina, Kevin Solutions
    drawtext(125, 130, " © MAKS Maps");
    
    drawSearchBox();
    set_coordinate_system(GL_SCREEN);
    
    //#1 item
    //highlighting
    if (mouseOverExplore) {
        setcolor(189, 156, 209);
        fillrect(0, 157, 370, 241);
    }
    //text and icon
    Surface explore("/homes/x/xuwenkai/ECE297/mapper/explore-tool.png");
    draw_surface(explore, 20, 185);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70, 199, "Explore", 9999, 9999);
    
    //Public Transit
    if (mouseOverTransit) {
        setcolor(189, 156, 209);
        fillrect(0, 242, 370, 326);
    }
    Surface transit("/homes/x/xuwenkai/ECE297/mapper/minibus.png");
    draw_surface(transit, 20, 270);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70, 284, "Public Transit", 9999, 9999);
    
    //System
    if (mouseOverSystem) {
        setcolor(189, 156, 209);
        fillrect(0, 327, 370, 411);
    }
    //text and icon
    Surface system("/homes/x/xuwenkai/ECE297/mapper/settings.png");
    draw_surface(system, 20, 355);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70, 369, "System", 9999, 9999);
    
    //About
    if (mouseOverAbout) {
        setcolor(189, 156, 209);
        fillrect(0, 412, 370, 495);
    }
    //text and icon
    Surface about("/homes/x/xuwenkai/ECE297/mapper/settings.png");
    draw_surface(about, 20, 440);
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(70, 450, "About MAKS GIS", 9999, 9999);
    
    copy_off_screen_buffer_to_screen();
    set_coordinate_system(GL_WORLD);
}


void drawExploreMenu(bool redraw) {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    
    //shadow
    if (!redraw && !bottomMenuShadow) {
        setcolor(0, 0, 0, 10);
        for (unsigned i = 0; i < 10; ++ i) {
            fillrect(600, 0, 600 + i, windowHeight);
        }
        bottomMenuShadow = true;
    }
    
    setcolor(250, 250, 250);
    fillrect(370, 0, 600, windowHeight);
    
    drawExploreItem("Restaurant", 30);
    drawExploreItem("Fun Place", 70);
    drawExploreItem("Ice Cream", 110);
    drawExploreItem("Cafe", 150);
    drawExploreItem("Psychic", 190);
    drawExploreItem("Health Care", 230);
    drawExploreItem("Telephone", 270);
    drawExploreItem("Condo", 310);
    drawExploreItem("SPA", 350);
    drawExploreItem("Pharmacy", 390);
    drawExploreItem("Parking", 430);
    drawExploreItem("Bank", 470);
    drawExploreItem("College", 510);
    drawExploreItem("Childcare", 550);
    drawExploreItem("Library", 590);
    drawExploreItem("Pub", 630);
    drawExploreItem("Lab", 670);
    drawExploreItem("School", 710);
    drawExploreItem("Lawyer", 750);
    drawExploreItem("Entertainment", 790);
    drawExploreItem("ATM", 830);
    drawExploreItem("Police", 870);
    drawExploreItem("Gas", 910);
    
    set_coordinate_system(GL_WORLD);
}


void drawTramsitMenu(bool redraw) {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    
    //shadow
    if (!redraw && !bottomMenuShadow) {
        setcolor(0, 0, 0, 10);
        for (unsigned i = 0; i < 10; ++ i) {
            fillrect(600, 0, 600 + i, windowHeight);
        }
        bottomMenuShadow = true;
    }
    
    setcolor(250, 250, 250);
    fillrect(370, 0, 600, windowHeight);
    
    drawExploreItem("Subway", 30);
    drawExploreItem("Street Car", 70);
    drawExploreItem("Bus", 110);
    drawExploreItem("Ferry", 150);
    
    set_coordinate_system(GL_WORLD);
}


void drawSystemMenu(bool redraw) {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    
    //shadow 
    if (!redraw && !bottomMenuShadow) {
        setcolor(0, 0, 0, 10);
        for (unsigned i = 0; i < 10; ++ i) {
            fillrect(600, 0, 600 + i, windowHeight);
        }
        bottomMenuShadow = true;
    }
    
    setcolor(250, 250, 250);
    fillrect(370, 0, 600, windowHeight);
    
    drawExploreItem("Exit Map", 30);
    drawExploreItem("Zoom to Fit", 70);
    drawExploreItem("Save Current Map", 110);
    
    set_coordinate_system(GL_WORLD);
}


void drawAboutMenu(bool redraw) {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    
    //shadow 
    if (!redraw && !bottomMenuShadow) {
        setcolor(0, 0, 0, 10);
        for (unsigned i = 0; i < 10; ++ i) {
            fillrect(600, 0, 600 + i, windowHeight);
        }
        bottomMenuShadow = true;
    }
    
    setcolor(250, 250, 250);
    fillrect(370, 0, 600, windowHeight);
    
    drawExploreItem("Version V" + currentVersion, 30);
    drawExploreItem("Team", 70);
    drawExploreItem("Credits", 110);
    
    set_coordinate_system(GL_WORLD);
}


void drawExploreItem(std::string name, int y) {
    if ((exploreItem < y + 20 && exploreItem > y - 20)
            || (exploreItemON < y + 20 && exploreItemON > y - 20)) {
        setcolor(189, 156, 209);
        fillrect(370, y - 20, 600, y + 20);
    }
    //text
    setcolor(128, 128, 128);
    settextattrs(14, 0);
    drawtext_left(410, y, name, 9999, 9999);
}

void act_on_exploreItem_press(bool& POIflag, int y) {
    if (exploreItem < y + 20 && exploreItem > y - 20) {
        if (POIflag) {
            POIflag= false; 
            exploreItemON = 0;
        }
        else POIflag= true;
        drawMap();
    }
}


void drawNevBar() {
    set_coordinate_system(GL_SCREEN);

    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    //shadow
    setcolor(0, 0, 0, 10);
    for (unsigned i = 0; i < 10; ++ i) {
        fillrect(0, 0, 371 + i, windowHeight);
    }
    
    setcolor(WHITE);
    fillrect(0, 0, 370, windowHeight);
    
    //input area
    setcolor(0,0,0,20);
    fillrect(362,155,370,windowHeight);
    setcolor(161, 113, 187);
    fillrect(0, 0, 370, 156);
    
    //now we print out travel directions
    std::vector<trunInfo> direcList = streetNet::getInstance()->path2direction(selectedSeg);
    unsigned idx = 0;
    unsigned ide = direcList.size()-1;
    
    
    if (scroll == -1) scrollOffset ++;
    else if (scroll == 1) scrollOffset --;
    scroll = 0;
    
    if (scrollOffset <= 0) scrollOffset = 0;
    if (scrollOffset >= (direcList.size())) scrollOffset = direcList.size() - 1;
    

    while (idx < (direcList.size()) - scrollOffset) {
        
        drawDirectionNode(idx, direcList[idx + scrollOffset],ide - scrollOffset);
        idx ++;
    }
    
    
    set_coordinate_system(GL_WORLD);
}

void drawDirectionNode(unsigned index, trunInfo& Info,unsigned inde) {
    set_coordinate_system(GL_SCREEN);
    
    std::string trun;
    if (Info.turnDirection == 0) {
        trun = "Left";
        Surface leftIcon("/homes/w/weiyuyao/mapper_repo/mapper/leftIcon.png");
        draw_surface(leftIcon, 20, 183 + index * 85);
        
    }
    else if (Info.turnDirection == 1) {
        trun = "Right";
        Surface leftIcon("/homes/w/weiyuyao/mapper_repo/mapper/right.png");
        draw_surface(leftIcon, 20, 183 + index * 85);
    }
    else if (Info.turnDirection == 2) {
        trun = "Stright";
        Surface leftIcon("/homes/w/weiyuyao/mapper_repo/mapper/upload.png");
        draw_surface(leftIcon, 20, 183 + index * 85);
    }
    
    setcolor(128, 128, 128);
    settextattrs(12, 0);
    if(int(Info.nextTurn)>=1000){
        double dis = pow(10.0,2);
        
        drawtext_left(80, 183 + index * 85, "In " + to_string(round(((Info.nextTurn)/1000)*dis)/dis) + "km", 9999, 9999);
    }
    
    else if (int(Info.nextTurn)<1000){
    drawtext_left(80, 183 + index * 85, "In " + to_string(int(Info.nextTurn)) + "m", 9999, 9999);
    }
    
    
    if(getStreetName(Info.stID) == "<unknown>"){
        if(index == inde){
            drawtext_left(80, 201 + index * 85, "Reach your destination", 9999, 9999);
            Surface flag("/homes/w/weiyuyao/mapper_repo/mapper/flag1.png");
            draw_surface(flag, 20, 180 + index * 85);
        }
        else{
        drawtext_left(80, 201 + index * 85, "Turn " + trun + " onto", 9999, 9999);
        drawtext_left(80, 216 + index * 85, "Upcoming Street", 9999, 9999);
        }
    }
    
    else {
    drawtext_left(80, 201 + index * 85, "Turn " + trun + " onto", 9999, 9999);
    drawtext_left(80, 216 + index * 85, getStreetName(Info.stID), 9999, 9999);
    }
    set_coordinate_system(GL_WORLD);
}


void showDotPath() {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    LatLon startLL(startLocation.second / DEG_TO_RAD, startLocation.first /(DEG_TO_RAD * cosAveLat));
    LatLon endLL(endLocation.second / DEG_TO_RAD, endLocation.first /(DEG_TO_RAD * cosAveLat));
    unsigned start = find_closest_intersection(startLL);
    unsigned end = find_closest_intersection(endLL);
    selectedInter = streetNet::getInstance()->
            getPathFromIntersec(start, end, truncost, demoing, AstartON);
    demoing = false;
}


void formEdgePath() {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    LatLon startLL(startLocation.second / DEG_TO_RAD, startLocation.first /(DEG_TO_RAD * cosAveLat));
    LatLon endLL(endLocation.second / DEG_TO_RAD, endLocation.first /(DEG_TO_RAD * cosAveLat));
    unsigned start = find_closest_intersection(startLL);
    unsigned end = find_closest_intersection(endLL);
    selectedSeg = streetNet::getInstance()
            ->nodePath2edgePath(streetNet::getInstance()
            ->getPathFromIntersec(start, end, truncost, demoing, AstartON),
            truncost);
//    selectedSeg = streetNet::getInstance()
//            ->getPathFromIntersec(start, end, 15, demoing, AstartON);
}


void showEdgePath() {
//    double cosAveLat = streetNet::getInstance()->getInterAveLat();
//    LatLon startLL(startLocation.second / DEG_TO_RAD, startLocation.first /(DEG_TO_RAD * cosAveLat));
//    LatLon endLL(endLocation.second / DEG_TO_RAD, endLocation.first /(DEG_TO_RAD * cosAveLat));
//    unsigned start = find_closest_intersection(startLL);
//    unsigned end = find_closest_intersection(endLL);
//    selectedSeg = streetNet::getInstance()
//            ->nodePath2edgePath(streetNet::getInstance()
//            ->getPathFromIntersec(start, end, 15, demoing, AstartON));
    for (unsigned i = 0; i < selectedSeg.size(); ++ i) {
        streetNet::getInstance()->getSegByID(selectedSeg[i]).drawContour(true);
    }
    demoing = false;
}


void showRoute() {
    std::vector<DeliveryInfo> deliveries;
    std::vector<unsigned> depots;
    unsigned turn_penalty;
    
    //insert test package below ^^^^^^^^^^^^^^^^^^^^^^^^^^^
    deliveries = {DeliveryInfo(79796, 42839), DeliveryInfo(6620, 18908), DeliveryInfo(12917, 69324), DeliveryInfo(4009, 76027), DeliveryInfo(78640, 49449), DeliveryInfo(21674, 90763), DeliveryInfo(50896, 23880), DeliveryInfo(74090, 53653), DeliveryInfo(2111, 61148), DeliveryInfo(43971, 1223), DeliveryInfo(50063, 91913), DeliveryInfo(82083, 84759), DeliveryInfo(105386, 101774), DeliveryInfo(107769, 57682), DeliveryInfo(82981, 96244), DeliveryInfo(8738, 34980), DeliveryInfo(1342, 95587), DeliveryInfo(42483, 103377), DeliveryInfo(1161, 103664), DeliveryInfo(21428, 77598), DeliveryInfo(71316, 23235), DeliveryInfo(95619, 32911), DeliveryInfo(99061, 18848), DeliveryInfo(93949, 20039), DeliveryInfo(67982, 12095), DeliveryInfo(11945, 105365), DeliveryInfo(94347, 20391), DeliveryInfo(92583, 13875), DeliveryInfo(2811, 42442), DeliveryInfo(20951, 31516), DeliveryInfo(104712, 15493), DeliveryInfo(25003, 55427), DeliveryInfo(23213, 50781), DeliveryInfo(33297, 94451), DeliveryInfo(91792, 620), DeliveryInfo(94877, 17273), DeliveryInfo(42021, 30653), DeliveryInfo(45453, 80507), DeliveryInfo(19701, 9255), DeliveryInfo(100880, 11642), DeliveryInfo(43478, 97117), DeliveryInfo(37661, 18514), DeliveryInfo(27367, 105658), DeliveryInfo(4306, 54259), DeliveryInfo(107255, 358), DeliveryInfo(90985, 90550), DeliveryInfo(105483, 54918), DeliveryInfo(52218, 24878), DeliveryInfo(64146, 14569), DeliveryInfo(96695, 6359), DeliveryInfo(77408, 7132), DeliveryInfo(475, 94272), DeliveryInfo(94111, 10286), DeliveryInfo(102213, 28393), DeliveryInfo(31031, 5428), DeliveryInfo(61407, 87201), DeliveryInfo(52502, 25739), DeliveryInfo(64678, 11041), DeliveryInfo(7230, 43223), DeliveryInfo(42470, 32197), DeliveryInfo(17541, 79209), DeliveryInfo(38249, 15541), DeliveryInfo(44546, 79051), DeliveryInfo(11121, 102845), DeliveryInfo(55855, 31136), DeliveryInfo(38533, 45203), DeliveryInfo(82603, 34501), DeliveryInfo(84243, 24909), DeliveryInfo(69588, 744), DeliveryInfo(10546, 6312), DeliveryInfo(16898, 19546), DeliveryInfo(43784, 60601), DeliveryInfo(64789, 68213), DeliveryInfo(28003, 21779), DeliveryInfo(17561, 48204), DeliveryInfo(69514, 34766), DeliveryInfo(17040, 74080), DeliveryInfo(91356, 102978), DeliveryInfo(6292, 87024), DeliveryInfo(65826, 27845), DeliveryInfo(80009, 86251), DeliveryInfo(105642, 44253), DeliveryInfo(16066, 8023), DeliveryInfo(20106, 87653), DeliveryInfo(66771, 16683), DeliveryInfo(90038, 88410), DeliveryInfo(40917, 83054), DeliveryInfo(60050, 94321), DeliveryInfo(55413, 11814), DeliveryInfo(56359, 1693), DeliveryInfo(23931, 47427), DeliveryInfo(86665, 93996), DeliveryInfo(79695, 16400), DeliveryInfo(58624, 21665), DeliveryInfo(82028, 43899), DeliveryInfo(30893, 19071), DeliveryInfo(31114, 26049), DeliveryInfo(31066, 56617), DeliveryInfo(103338, 92746), DeliveryInfo(7629, 13181)};
    depots = {8552, 38211, 92758, 39860, 100277, 17123, 6130, 40760, 5413, 47813};
    turn_penalty = 15;
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
    selectedSeg = streetNet::getInstance()
            ->nodePath2edgePath(streetNet::getInstance()
            ->findRouteO3(deliveries, depots, turn_penalty), turn_penalty);
    
//    streetNet::getInstance()
//            ->findRoute(deliveries, depots, turn_penalty);
//    //draw all the stops
//    for (unsigned i = 0; i < deliveries.size(); ++ i) {
//        streetNet::getInstance()->getIntersecByID(deliveries[i].pickUp).highlightPoint();
//        streetNet::getInstance()->getIntersecByID(deliveries[i].dropOff).highlightPoint();
//    }
}


void drawCityName() {
    double centerY = (streetNet::getInstance()->getRange(1, 0).second 
                        + streetNet::getInstance()->getRange(0, 1).second) /2 ;
    double centerX = (streetNet::getInstance()->getRange(1, 0).first 
                        + streetNet::getInstance()->getRange(0, 1).first) /2 ;
    int windowHeight = -get_visible_screen().get_height();
//    
//    unsigned size = (1/windowHeight) * 0.1;
    
    setcolor(0, 0, 0, 128);
    settextattrs(32, 0);
    drawtext(centerX, centerY, cityName);
}


void motionAnimation(t_bound_box start, t_bound_box end, unsigned duration) {
    //this transition is forced to be 30fps (33ms / frame)
    unsigned frameCount = duration / 33;
    
    //Top left point
    double dxTL = (end.left() - start.left()) / frameCount;
    double dyTL = (end.top() - start.top()) / frameCount;
    //Bottom right point
    double dxBR = (end.right() - start.right()) / frameCount;
    double dyBR = (end.bottom() - start.bottom()) / frameCount;
    
    for (unsigned f = 0; f <= frameCount; ++ f) {
        double TLx = start.left() + (dxTL * f);
        double TLy = start.top() + (dyTL * f);
        double BRx = start.right() + (dxBR * f);
        double BRy = start.bottom() + (dyBR * f);
        
        t_bound_box current(TLx, BRy, BRx, TLy);
        set_visible_world(current);
        drawMap();
        delay(1);
    }
    
}


void drawStatic() {
    set_coordinate_system(GL_WORLD); //this should be default
    streetNet::getInstance()->drawFeature();
    streetNet::getInstance()->drawAllSeg();
    streetNet::getInstance()->drawPOI();
    
    if (debugMode && selectedInter.size() > 1) highlightSegs();
    if (findPath) showEdgePath();
    
    drawZoomButtom();
    drawScaleBar();
    
    drawSearchBoxShadow();
    if (searchNevMod) drawNevSearchBoxShadow();
    
    if (nevBar) drawNevBar();

    bottomMenuShadow = false;
    if (menuOut) {
        drawSysMenu(false, false);
    }
    drawSearchBox();
    if (searchNevMod || nevBar) drawNevSearchBox();
    
    
    drawSearchBoxShadow();
    drawSearchBox();
}


void drawMap() {
    set_coordinate_system(GL_WORLD); //this should be default
    
    //this part stitches the map together
    #ifdef drawperf
        auto const costStart = std::chrono::high_resolution_clock::now();
    #endif
    streetNet::getInstance()->drawFeature();
    streetNet::getInstance()->drawAllSeg();
    streetNet::getInstance()->drawPOI();
    #ifdef drawperf
        auto const costEnd = std::chrono::high_resolution_clock::now();
        auto const costTime = 
        std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
        std::cout << "drawing took: " << costTime.count() << "ms" << endl;
    #endif
    if (debugMode && selectedInter.size() > 1) highlightSegs();
    if (findPath) showEdgePath();
//    drawPinAnimate();  // to slow to use
    drawPin();
    if (startLocation.first != 0 && startLocation.second != 0) {
        drawStart();
    }
    if (endLocation.first != 0 && endLocation.second != 0) {
        drawEnd();
    }
    if (findPath) path2snackBar();
    if (intersecSnack != 0) intersec2snackbar();
    if (POISnack != 0) POI2snackbar();
    
    
    drawZoomButtom();
    drawScaleBar();
//    drawPOIMenu();
    
    drawSearchBoxShadow();
    if (searchNevMod) drawNevSearchBoxShadow();
    
    if (nevBar) drawNevBar();

    bottomMenuShadow = false;
    if (menuOut) {
        drawSysMenu(false, false);
    }
    drawSearchBox();
    if (searchNevMod || nevBar) drawNevSearchBox();
     
    if (rightClickMenu.first != 0 && rightClickMenu.second != 0) drawRightClickMenu(false);
    
//    if (NegMenu) drawNavigationMenu();
    
    if (debugMode) debugInfo();
    
//    SysMenuAnimFrame(250);
    
    
//    drawCityName();
    copy_off_screen_buffer_to_screen();
}


void LoadingCity() {
    set_drawing_buffer(OFF_SCREEN);   
    set_coordinate_system(GL_SCREEN);
    
    Surface loadingscreen("/homes/x/xuwenkai/ECE297/mapper/start.png");
    draw_surface(loadingscreen, 0, 0);
    
    setcolor(0, 0, 0, 128);
    settextattrs(64, 0);
    drawtext(get_visible_screen().get_xcenter(), get_visible_screen().get_ycenter(), "LOADING...");
    proceed(drawMap);
    
    
    set_coordinate_system(GL_WORLD);
    copy_off_screen_buffer_to_screen();
}


void drawAll() {
    
    t_color backGrondColour(224, 224, 215);
    init_graphics("MAKS GIS 2017", backGrondColour);  //initializing

    LoadingCity();
    if (!mapLoaded) {
    //loadOSMDatabaseBIN("/cad2/ece297s/public/maps/" + pathOSM);
    load_map("/cad2/ece297s/public/maps/" + pathSt);
    }
    
    set_drawing_buffer(OFF_SCREEN);                 //start double buffer drawing
    set_visible_world(getMapRange());              //set coords range

    drawMap();
    set_keypress_input(true);
    set_mouse_move_input(true);
    
    exitEvent = false;
    event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, drawMap);       //pass control

    
    close_map();
//    closeOSMDatabase();
    resetGloble();
    redrawAll();
    
    
    close_graphics();
    
    close_map();
    closeOSMDatabase();
}

void redrawAll() {
//    if (streetNet::getInstance() == NULL) {
    //loadOSMDatabaseBIN("/cad2/ece297s/public/maps/" + pathOSM);
    load_map("/cad2/ece297s/public/maps/" + pathSt);
//    }

    set_visible_world(getMapRange());              //set coords range
    drawMap();

    exitEvent = false;
    event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, drawMap);       //pass control

    close_map();
//    closeOSMDatabase();
    resetGloble();
    redrawAll();
}


void systemChecker() {
    if (quitFlag) quit(drawMap);
    if (zoom_fitFlag) zoom_fit(drawMap);
    if (postscriptFlag) postscript(drawMap);
    
    quitFlag = false;
    zoom_fitFlag = false;
    postscriptFlag = false;
}


void debugInfo() {
    set_coordinate_system(GL_SCREEN);
    
    int windowHeight = -get_visible_screen().get_height();
    int windowWidth = get_visible_screen().get_width();
    
    setcolor(WHITE);
    fillrect(windowWidth -30, 15, windowWidth -170, 200);
    
    
    setcolor(RED);
    settextattrs(10, 0);
    drawtext(windowWidth -100, 20, "DEBUG MODE");
    settextattrs(8, 0);
    drawtext(windowWidth -100, 35, "Keyboard Input:");
    drawtext(windowWidth -100, 45, keyboardInput);
    drawtext(windowWidth -100, 55, "Mouse Pos Scrn:");
    drawtext(windowWidth -100, 65, std::to_string(0));
    drawtext(windowWidth -100, 75, "Mouse Pos World:");
    drawtext(windowWidth -100, 85, std::to_string(mouseOverRightClickMenu));
    drawtext(windowWidth -100, 95, "Segment Info:"); 
    if (debugMode && selectedInter.size() > 1) {
        unsigned newSeg = streetNet::getInstance()->
            findSegByInter(selectedInter[selectedInter.size()-2],selectedInter[selectedInter.size()-1]);
        std::string stname = streetNet::getInstance()->getSegByID(newSeg).getStName();
        drawtext(windowWidth -100, 105, stname);
        drawtext(windowWidth -100, 115, std::to_string(newSeg));
        drawtext(windowWidth -100, 125,
                to_string(streetNet::getInstance()->getSegByID(newSeg).getTravelTime()));
        drawtext(windowWidth -100, 135,
                to_string(streetNet::getInstance()->getSegByID(newSeg).setSpeedLimit()));
    }
    drawtext(windowWidth -100, 145, "Total travel time:");
//    vector<unsigned> selectedSeg;
//    for (unsigned i = 1; i < selectedInter.size(); ++ i) {
//        unsigned segment = streetNet::getInstance()->
//            findSegByInter(selectedInter[i-1], selectedInter[i]);
//        selectedSeg.push_back(segment);
//    }
    double time = streetNet::getInstance()->pathTravelTime(selectedSeg, truncost);
    drawtext(windowWidth -100, 155, std::to_string(time));
    
    drawtext(windowWidth -100, 175, "zoom level");
    drawtext(windowWidth -100, 185, std::to_string(get_visible_world().get_height()));
   
    set_coordinate_system(GL_WORLD);
}


void textParser() {  //*********************************************************
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    std::string torontoSt = "toronto_canada.streets.bin";
    std::string torontoOSM = "toronto_canada.osm.bin";
    
    std::string newyorkSt = "new-york_usa.streets.bin";
    std::string newyorkOSM = "new-york_usa.osm.bin";
    
    std::string beijingSt = "beijing_china.streets.bin";
    std::string beijingOSM = "beijing_china.osm.bin";
    
    std::string cairoSt = "cairo_egypt.streets.bin";
    std::string cairoOSM = "cairo_egypt.osm.bin";
    
    std::string capetownSt = "cape-town_south-africa.streets.bin";
    std::string capetownOSM = "cape-town_south-africa.osm.bin";
    
    std::string goldenhorseshoeSt = "golden-horseshoe_canada.streets.bin";
    std::string goldenhorseshoeOSM = "golden-horseshoe_canada.osm.bin";
    
    std::string hamiltonSt = "hamilton_canada.streets.bin";
    std::string hamiltonOSM = "hamilton_canada.osm.bin";
    
    std::string hongkongSt = "hong-kong_china.streets.bin";
    std::string hongkongOSM = "hong-kong_china.osm.bin";
    
    std::string icelandSt = "iceland.streets.bin";
    std::string icelandOSM = "iceland.osm.bin";
    
    std::string londonSt = "london_england.streets.bin";
    std::string londonOSM = "london_england.osm.bin";
    
    std::string moscowSt = "moscow_russia.streets.bin";
    std::string moscowOSM = "moscow_russia.osm.bin";
    
    std::string newdelhiSt = "new-delhi_india.streets.bin";
    std::string newdelhiOSM = "new-delhi_india.osm.bin";
    
    std::string riodejaneiroSt = "rio-de-janeiro_brazil.streets.bin";
    std::string riodejaneiroOSM = "rio-de-janeiro_brazil.osm.bin";
    
    std::string sainthelenaSt = "saint-helena.streets.bin";
    std::string sainthelenaOSM = "saint-helena.osm.bin";
    
    std::string singaporeSt = "singapore.streets.bin";
    std::string singaporeOSM = "singapore.osm.bin";
    
    std::string sydneySt = "sydney_australia.streets.bin";
    std::string sydneyOSM = "sydney_australia.osm.bin";
    
    std::string tehranSt = "tehran_iran.streets.bin";
    std::string tehranOSM = "tehran_iran.osm.bin";
    
    std::string tokyoSt = "tokyo_japan.streets.bin";
    std::string tokyoOSM = "tokyo_japan.osm.bin";
    
    //parce search input
    std::stringstream inputvalue(keyboardInput); 
    vector<std::string> parsed; 
    while (!inputvalue.eof()) {
        std::string newstr;
        std::getline(inputvalue, newstr, ',');
        parsed.push_back(newstr);
    }
    
    //parce second input
    std::stringstream inputvalue2(secondInput); 
    vector<std::string> parsed2; 
    if (secondInput.compare("") != 0) {
        while (!inputvalue2.eof()) {
            std::string newstr;
            std::getline(inputvalue2, newstr, ',');
            parsed2.push_back(newstr);
        }
    }
      
//    std::getline(inputvalue, parsed2);
    std::vector<unsigned> stList;
    if (parsed.size() > 1) {
        stList = find_intersection_ids_from_street_names(parsed[0], parsed[1]);
    }
    
    std::vector<unsigned> stList2;
    if (parsed2.size() > 1)
        stList2 = find_intersection_ids_from_street_names(parsed2[0], parsed2[1]);
    
    //--------------------------------------------------
    if (keyboardInput.compare("toronto") == 0) {
        pathSt = torontoSt;
        pathOSM = torontoOSM;
        proceed(drawMap);
        make_a_toast("Loading Toronto...", 9999);
    }
    else if (keyboardInput.compare("newyork") == 0) {
        pathSt = newyorkSt;
        pathOSM = newyorkOSM;
        proceed(drawMap);
        make_a_toast("Loading NewYork...", 9999);
    }
    
    else if (keyboardInput.compare("beijing") == 0) {
        pathSt = beijingSt;
        pathOSM = beijingOSM;
        proceed(drawMap);
        make_a_toast("Loading Beijing...", 9999);
    }
    
    else if (keyboardInput.compare("tokyo") == 0) {
        pathSt = tokyoSt;
        pathOSM = tokyoOSM;
        proceed(drawMap);
        make_a_toast("Loading Tokyo...", 9999);
    }
    
    else if (keyboardInput.compare("tehran") == 0) {
        pathSt = tehranSt;
        pathOSM = tehranOSM;
        proceed(drawMap);
        make_a_toast("Loading Tehran...", 9999);
    }
    
    else if (keyboardInput.compare("sydney") == 0) {
        pathSt = sydneySt;
        pathOSM = sydneyOSM;
        proceed(drawMap);
        make_a_toast("Loading Sydney...", 9999);
    }
    
    else if (keyboardInput.compare("singapore") == 0) {
        pathSt = singaporeSt;
        pathOSM = singaporeOSM;
        proceed(drawMap);
        make_a_toast("Loading Singpore...", 9999);
    }
    
    else if (keyboardInput.compare("saint helena") == 0) {
        pathSt = sainthelenaSt;
        pathOSM = sainthelenaOSM;
        proceed(drawMap);
        make_a_toast("Loading Saint-Helena...", 9999);
    }
    
    else if (keyboardInput.compare("rio de janeiro") == 0) {
        pathSt = riodejaneiroSt;
        pathOSM = riodejaneiroOSM;
        proceed(drawMap);
        make_a_toast("Loading RIO...", 9999);
    }
    
    else if (keyboardInput.compare("new delhi") == 0) {
        pathSt = newdelhiSt;
        pathOSM = newdelhiOSM;
        proceed(drawMap);
        make_a_toast("Loading NewDelhi...", 9999);
    }
    
    else if (keyboardInput.compare("moscow") == 0) {
        pathSt = moscowSt;
        pathOSM = moscowOSM;
        proceed(drawMap);
        make_a_toast("Loading Moscow...", 9999);
    }
    
    else if (keyboardInput.compare("london") == 0) {
        pathSt = londonSt;
        pathOSM = londonOSM;
        proceed(drawMap);
        make_a_toast("Loading London...", 9999);
    }
    

    else if (keyboardInput.compare("hong kong") == 0) {
        pathSt = hongkongSt;
        pathOSM = hongkongOSM;
        proceed(drawMap);
        make_a_toast("Loading Hong Kong...", 9999);
    }
    
    else if (keyboardInput.compare("iceland") == 0) {
        pathSt = icelandSt;
        pathOSM = icelandOSM;
        proceed(drawMap);
        make_a_toast("Loading Iceland...", 9999);
    }
    
    else if (keyboardInput.compare("hamilton") == 0) {
        pathSt = hamiltonSt;
        pathOSM = hamiltonOSM;
        proceed(drawMap);
        make_a_toast("Loading Hamilton...", 9999);
    }
    
    else if (keyboardInput.compare("golden-horseshoe") == 0) {
        pathSt = goldenhorseshoeSt;
        pathOSM = goldenhorseshoeOSM;
        proceed(drawMap);
        make_a_toast("Loading GTA...", 9999);
    }
    
    else if (keyboardInput.compare("cairo") == 0) {
        pathSt = cairoSt;
        pathOSM = cairoOSM;
        proceed(drawMap);
        make_a_toast("Loading Cairo...", 9999);
    }
    
    else if (keyboardInput.compare("capetown") == 0) {
        pathSt = capetownSt;
        pathOSM = capetownOSM;
        proceed(drawMap);
        make_a_toast("Loading Capetown...", 9999);
    }
    
    //this search features
    else if (streetNet::getInstance()->getFeatureByName(keyboardInput).size() != 0) {
        unsigned featureID = streetNet::getInstance()->getFeatureByName(keyboardInput)[0];
        
        PinLocation.first = streetNet::getInstance()
            ->getFeasture(featureID).getLocation().lon() * DEG_TO_RAD * cosAveLat;
        PinLocation.second = streetNet::getInstance()
            ->getFeasture(featureID).getLocation().lat() * DEG_TO_RAD;
        
        drawPin();
        
        t_bound_box newWindow(PinLocation.first - 0.0001, PinLocation.second - 0.0001
                            , PinLocation.first + 0.0001, PinLocation.second + 0.0001);
        set_visible_world(newWindow);
    }
    
    
    else if (keyboardInput.compare("debug mode") == 0 
                || keyboardInput.compare("debug mode on") == 0) {
        make_a_toast("Debug Mode ON");
        debugMode = true;
        drawMap();
    }
    else if (keyboardInput.compare("debug mode off") == 0) {
        make_a_toast("Debug Mode OFF");
        debugMode = false;
        drawMap();
    }
    else if (keyboardInput.compare("clear path") == 0 && debugMode) {
        selectedInter.clear();
        startLocation = {0, 0};
        endLocation = {0, 0};
        findPath = false;
        drawMap();
    }
    else if (keyboardInput.compare("demo") == 0 && debugMode) {
        demoing = true; 
        make_a_toast("Path Finding Demo Mode");
    }
    else if (keyboardInput.compare("a* on") == 0 && debugMode) {
        AstartON = true; 
        make_a_toast("Switching to A* search");
    }
    else if (keyboardInput.compare("a* off") == 0 && debugMode) {
        AstartON = false; 
        make_a_toast("Switching to Dijkstra’s search");
    }
    else if (parsed[0].compare("intersec") == 0 && debugMode) { //--------------intersec commond
        unsigned newID = atoi(parsed[1].c_str());
        locate_intersection(newID);
        drawMap();
    }
    else if (parsed[0].compare("path") == 0 && debugMode && parsed.size() > 2) { //POI path finding commond
        unsigned newID = atoi(parsed[1].c_str());
        selectedSeg = streetNet::getInstance()->nodePath2edgePath(streetNet::getInstance()->inter2POIpath(newID, parsed[2], truncost),truncost);
        findPath = true;
        drawMap();
    }
    
    else if (parsed[0].compare("turncost") == 0 && debugMode) { //--------------intersec commond
        unsigned newcost = atoi(parsed[1].c_str());
        truncost = newcost;
        drawMap();
    }
    
    else if ((stList.size() != 0) && (!searchNevMod)) {
        locate_intersection(stList[0]);
        std::thread toaster(make_a_toast,"Searching...", 9999);
        toaster.join();
        drawMap();
    }
    else if ((stList.size() != 0) && (stList2.size() != 0) && searchNevMod && parsed2.size() != 1) {
        selectedSeg = streetNet::getInstance()
            ->nodePath2edgePath(streetNet::getInstance()
            ->getPathFromIntersec(stList[0], stList2[0], truncost, demoing, AstartON),
                truncost);
        findPath = true; 
        drawMap();
    }
    
    
    else if (keyboardInput.compare("menu") == 0) {
        menuOut = true;
        drawMap();
    }
    
    else if (keyboardInput.compare("zoom in") == 0) {
        zoom_in(drawMap);
    }
    else if (keyboardInput.compare("zoom out") == 0) {
        zoom_out(drawMap);
    }
    else if (keyboardInput.compare("zoom fit") == 0) {
        zoom_fit(drawMap);
    }
    else if (keyboardInput.compare("not zoom") == 0) {
        notzoom = true;
    }
    else if (keyboardInput.compare("proceed") == 0) {
        proceed(drawMap);
    }
    else if (keyboardInput.compare("save") == 0) {
        postscript(drawMap);
    }
    else if (keyboardInput.compare("exit") == 0
            || keyboardInput.compare("quit") == 0) {
        quit(drawMap);
    }
    
    else if (keyboardInput.compare("show route") == 0 && debugMode) {
        make_a_toast("Generating Path...", 9999);
        showRoute();
        findPath = true;
        drawMap();
    }
    
    else if (keyboardInput.compare("test animation") == 0) {
        motionAnimation(get_visible_world(), getMapRange());
    }
    
    
    //---------------------------------------------------------------------------
    else if (keyboardInput.compare("show subway") == 0
            || keyboardInput.compare("subway on") == 0) {
        streetNet::getInstance()->subwayON = true;
        make_a_toast("Subway ON");
    }
    else if (keyboardInput.compare("subway off") == 0) {
        streetNet::getInstance()->subwayON = false;
        make_a_toast("Subway OFF");
    }
    
    else if (keyboardInput.compare("show restaurant") == 0) {
        streetNet::getInstance()->restarantON = true;
        make_a_toast("Restaurant ON");
    }
    else if (keyboardInput.compare("restaurant off") == 0) {
        streetNet::getInstance()->restarantON = false;
        make_a_toast("Restaurant OFF");
    }
    
    else if (keyboardInput.compare("show fun place") == 0) {
        streetNet::getInstance()->funPlaceON = true;
        make_a_toast("Stripclub ON");
    }
    else if (keyboardInput.compare("fun place off") == 0) {
        streetNet::getInstance()->funPlaceON = false;
        make_a_toast("Stripclub OFF");
    }
    
    else if (keyboardInput.compare("show ice cream") == 0) {
        streetNet::getInstance()->icecreamON = true;
        make_a_toast("Ice Cream ON");
    }
    else if (keyboardInput.compare("ice cream off") == 0) {
        streetNet::getInstance()->icecreamON = false;
        make_a_toast("Ice Cream OFF");
    }
    
    else if (keyboardInput.compare("show cafe") == 0) {
        streetNet::getInstance()->cafeON = true;
        make_a_toast("Cafe ON");
    }
    else if (keyboardInput.compare("cafe off") == 0) {
        streetNet::getInstance()->cafeON = false;
        make_a_toast("Cafe OFF");
    }
    
    else if (keyboardInput.compare("show dentist") == 0) {
        streetNet::getInstance()->dentistON = true;
        make_a_toast("Dentist ON");
    }
    else if (keyboardInput.compare("dentist off") == 0) {
        streetNet::getInstance()->dentistON = false;
        make_a_toast("Dentist OFF");
    }
    
    else if (keyboardInput.compare("show hospital") == 0) {
        streetNet::getInstance()->hospitalON = true;
        make_a_toast("Hospital ON");
    }
    else if (keyboardInput.compare("hospital off") == 0) {
        streetNet::getInstance()->hospitalON = false;
        make_a_toast("Hospital OFF");
    }
    
    else if (keyboardInput.compare("show doctors") == 0) {
        streetNet::getInstance()->doctorsON = true;
        make_a_toast("Doctors ON");
    }
    else if (keyboardInput.compare("doctors off") == 0) {
        streetNet::getInstance()->doctorsON = false;
        make_a_toast("Doctors OFF");
    }
    
    else if (keyboardInput.compare("show condo") == 0) {
        streetNet::getInstance()->condoON = true;
        make_a_toast("Condo ON");
    }
    else if (keyboardInput.compare("condo off") == 0) {
        streetNet::getInstance()->condoON = false;
        make_a_toast("Condo OFF");
    }
    
    else if (keyboardInput.compare("show clinic") == 0) {
        streetNet::getInstance()->clinicON = true;
        make_a_toast("Clinic ON");
    }
    else if (keyboardInput.compare("clinic off") == 0) {
        streetNet::getInstance()->clinicON = false;
        make_a_toast("Clinic OFF");
    }
    
    else if (keyboardInput.compare("show pharmacy") == 0) {
        streetNet::getInstance()->pharmacyON = true;
        make_a_toast("Pharmacy ON");
    }
    else if (keyboardInput.compare("pharmacy off") == 0) {
        streetNet::getInstance()->pharmacyON = false;
        make_a_toast("Pharmacy OFF");
    }
    
    else if (keyboardInput.compare("show parking") == 0) {
        streetNet::getInstance()->parkingON = true;
        make_a_toast("Parking ON");
    }
    else if (keyboardInput.compare("parking off") == 0) {
        streetNet::getInstance()->parkingON = false;
        make_a_toast("Parking OFF");
    }
    
    else if (keyboardInput.compare("show bank") == 0) {
        streetNet::getInstance()->bankON = true;
        make_a_toast("Bank ON");
    }
    else if (keyboardInput.compare("bank off") == 0) {
        streetNet::getInstance()->bankON = false;
        make_a_toast("Bank OFF");
    }
    
    
    else if (keyboardInput.compare("show college") == 0) {
        streetNet::getInstance()->collegeON = true;
        make_a_toast("College ON");
    }
    else if (keyboardInput.compare("college off") == 0) {
        streetNet::getInstance()->collegeON = false;
        make_a_toast("College OFF");
    }
    
    
    else if (keyboardInput.compare("show childcare") == 0) {
        streetNet::getInstance()->childcareON = true;
        make_a_toast("Childcare ON");
    }
    else if (keyboardInput.compare("childcare off") == 0) {
        streetNet::getInstance()->childcareON = false;
        make_a_toast("Childcare OFF");
    }
    
    
    else if (keyboardInput.compare("show library") == 0) {
        streetNet::getInstance()->libraryON = true;
        make_a_toast("Library ON");
    }
    else if (keyboardInput.compare("library off") == 0) {
        streetNet::getInstance()->libraryON = false;
        make_a_toast("Library OFF");
    }
    
    
    else if (keyboardInput.compare("show pub") == 0) {
        streetNet::getInstance()->pubON = true;
        make_a_toast("Pub ON");
    }
    else if (keyboardInput.compare("pub off") == 0) {
        streetNet::getInstance()->pubON = false;
        make_a_toast("Pub OFF");
    }
    
    
    else if (keyboardInput.compare("show bar") == 0) {
        streetNet::getInstance()->barON = true;
        make_a_toast("Bar ON");
    }
    else if (keyboardInput.compare("bar off") == 0) {
        streetNet::getInstance()->barON = false;
        make_a_toast("Bar OFF");
    }
    
    
    else if (keyboardInput.compare("show lab") == 0) {
        streetNet::getInstance()->labON = true;
        make_a_toast("Lab ON");
    }
    else if (keyboardInput.compare("lab off") == 0) {
        streetNet::getInstance()->labON = false;
        make_a_toast("Lab OFF");
    }
    
    
    else if (keyboardInput.compare("show school") == 0) {
        streetNet::getInstance()->schoolON = true;
        make_a_toast("School ON");
    }
    else if (keyboardInput.compare("school off") == 0) {
        streetNet::getInstance()->schoolON = false;
        make_a_toast("School OFF");
    }
    
    else if (keyboardInput.compare("show lawyer") == 0) {
        streetNet::getInstance()->lawyerON = true;
        make_a_toast("Lawyer ON");
    }
    else if (keyboardInput.compare("lawyer off") == 0) {
        streetNet::getInstance()->lawyerON = false;
        make_a_toast("Lawyer OFF");
    }
    
    else if (keyboardInput.compare("show theatre") == 0) {
        streetNet::getInstance()->theatreON = true;
        make_a_toast("Theatre ON");
    }
    else if (keyboardInput.compare("theatre off") == 0) {
        streetNet::getInstance()->theatreON = false;
        make_a_toast("Theatre OFF");
    }
    
    
   else if (keyboardInput.compare("show atm") == 0) {
        streetNet::getInstance()->atmON = true;
        make_a_toast("Atm ON");
    }
    else if (keyboardInput.compare("atm off") == 0) {
        streetNet::getInstance()->atmON = false;
        make_a_toast("Atm OFF");
    }
    
    else if (keyboardInput.compare("show police") == 0) {
        streetNet::getInstance()->policeON = true;
        make_a_toast("Police ON");
    }
    else if (keyboardInput.compare("police off") == 0) {
        streetNet::getInstance()->policeON = false;
        make_a_toast("Police OFF");
    }
    
    else if (keyboardInput.compare("show fuel") == 0) {
        streetNet::getInstance()->fuelON = true;
        make_a_toast("Fuel ON");
    }
    else if (keyboardInput.compare("fuel off") == 0) {
        streetNet::getInstance()->fuelON = false;
        make_a_toast("Fuel OFF");
    }
    
    else if (keyboardInput.compare("show veterinary") == 0) {
        streetNet::getInstance()->veterinaryON = true;
        make_a_toast("Veterinary ON");
    }
    else if (keyboardInput.compare("veterinary off") == 0) {
        streetNet::getInstance()->veterinaryON = false;
        make_a_toast("Veterinary OFF");
    }
    
    else if (keyboardInput.compare("show cinema") == 0) {
        streetNet::getInstance()->cinemaON = true;
        make_a_toast("Cinema ON");
    }
    else if (keyboardInput.compare("cinema off") == 0) {
        streetNet::getInstance()->cinemaON = false;
        make_a_toast("Cinema OFF");
    }
    
    else if (keyboardInput.compare("show tutoring") == 0) {
        streetNet::getInstance()->tutoringON = true;
        make_a_toast("Tutoring ON");
    }
    else if (keyboardInput.compare("tutoring off") == 0) {
        streetNet::getInstance()->tutoringON = false;
        make_a_toast("Tutoring OFF");
    }
    
    else if (keyboardInput.compare("show psychic") == 0) {
        streetNet::getInstance()->psychicON = true;
        make_a_toast("Psychic ON");
    }
    else if (keyboardInput.compare("psychic off") == 0) {
        streetNet::getInstance()->psychicON = false;
        make_a_toast("Psychic OFF");
    }
    
    else if (keyboardInput.compare("show telephone") == 0) {
        streetNet::getInstance()->telephoneON = true;
        make_a_toast("Telephone ON");
    }
    else if (keyboardInput.compare("telephone off") == 0) {
        streetNet::getInstance()->telephoneON = false;
        make_a_toast("Telephone OFF");
    }
    
    else if (keyboardInput.compare("show spa") == 0) {
        streetNet::getInstance()->spaON = true;
        make_a_toast("Spa ON");
    }
    else if (keyboardInput.compare("spa off") == 0) {
        streetNet::getInstance()->spaON = false;
        make_a_toast("Spa OFF");
    }
     
    else if (keyboardInput.compare("show kindergarten") == 0) {
        streetNet::getInstance()->kindergartenON = true;
        make_a_toast("Kindergarten ON");
    }
    else if (keyboardInput.compare("kindergarten off") == 0) {
        streetNet::getInstance()->kindergartenON = false;
        make_a_toast("Kindergarten OFF");
    }
    
    else make_a_toast("You can't have your cake and eat it, too");
}
