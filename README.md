# Matrix-Project

After weeks of working, it's finally here. 

<p><b>LCD functionalities</b></p>
 1. Intro Message - When powered, a greeting message is shown on the LCD
 2. Menu - user can scroll using joystick through the options displayed and choose what they want<br>
    a. Start Game, the game is displayed on the matrix<br>
    b. Settings, with 4 options itself, set intensity to LCD, set intensity to matrix, return to menu, enter name<br>
    c. About - details, it displays the github of the creator (mine, obviously)<br>
    d. HighScore <br>
 3. End Message -  it displays 'GAME OVER' and the score, goes back to menu
 4. During Gameplay - it displays the lives of the player and the time since the game began <br>
<b>Components used</b> <br>
Joystick 🕹️  <br>
8x8 LED Matrix  <br>
Resistors, capacitors and wires 🪛 <br>
LED💡 <br>
Buzzer 🔉<br>
LCD display 📺
<br>

<p><b>Everybody's favorite thing ever!! Rules</b></p>
<details>
  A bunch of things that might not be clear when first starting the game. When the setup is powered, there is a message displayed on the LCD for no more than 12 seconds. The most important thing it sasys it's that the user can scroll the menu using the joystick by moving it upwards or downwards. 
When the menu appears the first options that can be seen is a) Start game, that if chosen, start the game on the matrix. By scrolling you can see the other options, b)Settings and c)About. To choose an option of the MAIN MENU(read that again), the user needs to press the joystick once. <br>
☁️ Keep reading 🪗 <br>
If the first option is chosen, the game starts and the LCD display the current lives of the player and the timer of the game. For the second option, the settings, new options will be displayed. Again, the user can scroll through them using the up or down motion on the joystick. <br>
To chose an option the user has to swipe right with the joystick. The third option of this submenu allows the user to go back to the MAIN MENU. If user wants to set the intensity of the LCD, (and for the next assigment, maybe the intensity of the matrix), they swipe right on that option of the settings submenu. To set the intensity, they have to swipe the joystick left or right this time, and to swipe down to save the value to EEPROM and return to submenu (it was supposed to be up, down and right to leave as well, but wasn't working, sooo yeah). <br>
☁️ Keep going 🪗 <br>

The ABOUT option of the MAIN MENU is the nicest, it displays the GITHUB username of me. To return to MAIN MENU, swipe right. So easy! <br> <br>
<b>How to play!</b><br>
When user presses the Start Game option, the walls that needs to be distroyed are shown on the matrix. That one spot that goes blink is the player. With the joystick, the player can go UP, DOWN, LEFT, RIGHT, based on what spaces are around them. The player starts with a basic lifecount of 3. One press of the button places the bomb and the led lights up green. When player moves from where they placed the bomb user can see a new spot that goes blink-blink faster than the just blink of the player.<br> With another press of the button, the bomb is detonated and the walls that are on its direction get distroyed. If the player is in close proximity of the bomb, as in right next to it, or right on top of it, the player loses a life. When there are no more lives, the game is over! <br>
☁️ Just a bit more 🪗 <br>

The target of this game is to distroy all the walls. Be aware, the clock is ticking!<br>
After the game is terminated, the MAIN MENU is displayed. <br>
☁️ Last thing, I promise 🪗 <br>

I know rules are long and might be boring to read, but give it a try. If you don't, don't blame me when you won't know to handle the menu on the LCD! 🦝
</details>

<p><b>Setup of the assignment 📷 </b></p>
<img src="https://github.com/nicoletaiova25/IntroductionToRobotics/assets/148574222/f20ebd7f-653d-462d-adec-c74624af9f11).jpg" width="500" height="600">
<br>
