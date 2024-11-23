#define _USE_MATH_DEFINES
#include <cmath>
#include < stdlib.h >
#include <stdio.h>
#include <string.h>
#include <glut.h>


float gameTime = 0.0f;
bool isGameOver = false;
float spawnRate = 1.0f;
float CollectibleSpawnRate = 1.2f;
int health = 5;
int score = 0;
int playerX = 50;
int playerY = 100;
bool isJumping = false;
bool isDucking = false;
float jumpHeight = 50.0f;
float fallSpeed = 5.0f;
float jumpStartY = playerY;
float jumpVelocity = 0.0f;
const float gravity = -0.5f;
float screenWidth = 800;
float screenHeight = 600;
const int meteorCount = 10;
const int collectibleCount = 12;
bool invincibleFlag = false;
float invincibleDuration = 1.5f;
float invincibleTimer = 0.0f;
bool blinkingFlag = false;
float blinkingTimer = 0.0f;
int gameSpeedMultiplier = 500.0f;
int slowedDownGameSpeedMultiplier = 300;
int ySpawnPositions[3] = {100,180};
int meteorSize[3] = { 20, 30 };
bool spawnMeteorFlag = true;
float powerUpSpawnRate = 10.0f;
bool doubleScoreFlag = false;
float doubleScoreTimer = 0.0;
bool slowDownTimeFlag = false;
float slowDownTimer = 0.0f;
float minuteHandAngle = 0.0f;
float hourHandAngle = 0.0f;
float borderScale = 0.1f; // Scale factor for the border
float borderPulseSpeed = 0.02f; // Speed of the pulsing effect
bool borderGrowing = true; // Flag to track if the border is growing or shrinking
float gemShineTimer = 0.0f;
float shineDuration = 0.1f;
bool shineToggle = false;
bool isGameEnd = false;



struct Meteor {
	float x, y;
	float speed;
	float radius;
};

struct Collectible {
	float x, y;
	float speed;
};

enum PowerUpType {
	DOUBLESCORE,
	SLOWDOWNTIME
};

struct PowerUp {
	float x, y;
	float speed;
	PowerUpType type;
};

PowerUp powerUpArray[2]; // Adjust the size as needed
int activePowerUpCount = 0;


Collectible collectibleArray[collectibleCount];
int activeCollectibleCount = 0;

Meteor meteorsArray[meteorCount];
int activeMeteorCount = 0;

// Bounding Box Structure
struct BoundingBox {
	float x1, y1; // Top-left corner
	float x2, y2; // Bottom-right corner
};


// Function to check if two bounding boxes overlap
bool isCollision(const BoundingBox& box1, const BoundingBox& box2) {
	// Check if the boxes overlap
	if (box1.x1 > box2.x2 || box1.x2 < box2.x1 ||  // Horizontal non-overlap
		box1.y1 < box2.y2 || box1.y2 > box2.y1) {  // Vertical non-overlap
		return false;  // No collision
	}
	return true;  // Collision detected
}

// Function to create bounding box for the meteor
BoundingBox GetMeteorBoundingBox(const Meteor& meteor) {
	BoundingBox meteorBox;
	meteorBox.x1 = meteor.x - meteor.radius; // Left
	meteorBox.x2 = meteor.x + meteor.radius; // Right
	meteorBox.y1 = meteor.y + meteor.radius - 5; // Top
	meteorBox.y2 = meteor.y - meteor.radius; // Bottom
	return meteorBox;
}

BoundingBox GetCollectibleBoundingBox(const Collectible& collectible) {
	BoundingBox collectibleBox;
	collectibleBox.x1 = collectible.x+10;
	collectibleBox.x2 = collectible.x-10;
	collectibleBox.y1 = collectible.y+10;
	collectibleBox.y2 = collectible.y-10;
	return collectibleBox;
}

// Function to create bounding box for the player
BoundingBox GetDynamicPlayerBoundingBox() {
	BoundingBox playerBox;
	playerBox.x1 = playerX - 40/ 2;   // Left
	playerBox.x2 = playerX + 40 / 2;   // Right

	
	if (!isDucking) {
		playerBox.y1 = playerY + 120 / 2;  // Top
		playerBox.y2 = playerY - 120 / 2;  // Bottom
	}

	else {
		playerBox.y1 = playerY + 60 / 2;  // Adjusted top for ducking
		playerBox.y2 = playerY - 60/ 2;  // Adjusted bottom for ducking
	}

	return playerBox;
}


bool CheckPowerUpCollection(const PowerUp& powerUp) {
	BoundingBox powerUpBox;
	powerUpBox.x1 = powerUp.x - 10; // Adjust based on size
	powerUpBox.x2 = powerUp.x + 10;
	powerUpBox.y1 = powerUp.y + 20; // Adjust based on size
	powerUpBox.y2 = powerUp.y;

	BoundingBox playerBox = GetDynamicPlayerBoundingBox();

	return isCollision(powerUpBox, playerBox);
}


bool CheckCollision(const Meteor& meteor) {
	BoundingBox meteorBox = GetMeteorBoundingBox(meteor);
	BoundingBox playerBox = GetDynamicPlayerBoundingBox();
	return isCollision(meteorBox, playerBox);
}

bool CheckPickup(const Collectible& collectible) {
	BoundingBox collectibleBox = GetCollectibleBoundingBox(collectible);
	BoundingBox playerBox = GetDynamicPlayerBoundingBox();
	return isCollision(collectibleBox, playerBox);
}

void print(int x, int y, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
//how to print according to lab 
//glColor3f(1, 0, 0); give it a color
//char* p0s[20]; make char pointer to array
//sprintf((char*)p0s, "Score: %d", 50); 
//print(500, 300, (char*)p0s);

void checkGameOver() {
	if (health <= 0) {
		isGameOver = true;
	}
	if (score >= 30 ) {
		isGameEnd = true;
	}
	
}

void drawGameOver() {
	if (isGameOver) {
		glColor3f(1, 0, 0);
		char* text[20];
		sprintf((char*)text, "GAME OVER YOU DIED");
		print(screenWidth / 3, screenHeight / 2 +10, (char*)text);
	}
}

void drawGameEnd() {
	if (isGameEnd) {
		glColor3f(1, 0, 0);
		char* text[20];
		sprintf((char*)text, "GAME ENDED YOU SURVIVED");
		print(screenWidth / 3 , screenHeight / 2 +10, (char*)text);
	}
}


void spawnPowerUP() {
	if (activePowerUpCount < 2) { // Limit the number of active power-ups
		PowerUp newPowerUp;
		newPowerUp.x = screenWidth + 50; // Spawn offscreen
		newPowerUp.y = 130;
		newPowerUp.speed = gameSpeedMultiplier;

		// Randomly assign a power-up type
		if (rand() % 2 == 0) {
			newPowerUp.type = DOUBLESCORE;
		}
		else
		{
			newPowerUp.type = SLOWDOWNTIME;
		}
		powerUpArray[activePowerUpCount] = newPowerUp;
		activePowerUpCount++;
	}
}

void spawnCollectible() {
	if (activeCollectibleCount < collectibleCount) {
		Collectible newCollectible;
		newCollectible.x = screenWidth + 50;
		newCollectible.y = 130;
		newCollectible.speed = gameSpeedMultiplier;
		collectibleArray[activeCollectibleCount] = newCollectible;
		activeCollectibleCount++;
	}
}

void spawnMeteor() {
	if (activeMeteorCount < meteorCount) {
		//create new meteor
		Meteor newMeteor;
		//set position and speed and radius
		newMeteor.x = screenWidth + 50; // spawn it offscreen
		newMeteor.y = ySpawnPositions[rand() %2]; //random y need to make it set between 2 positions hi or low 
		newMeteor.radius = meteorSize[rand()%2];
		newMeteor.speed = gameSpeedMultiplier;
		meteorsArray[activeMeteorCount] = newMeteor;
		activeMeteorCount++;
	}
}


void updatePowerUpPosition(float deltaTime) {
	for (int i = 0; i < activePowerUpCount; i++) {
		if (!slowDownTimeFlag) {
			powerUpArray[i].x -= powerUpArray[i].speed * deltaTime;
		}
		else
		{
			powerUpArray[i].x -= slowedDownGameSpeedMultiplier * deltaTime;
		}


		if (powerUpArray[i].x < 0) { // Check if power-up is offscreen
			for (int j = i; j < activePowerUpCount - 1; j++) {
				powerUpArray[j] = powerUpArray[j + 1]; // Shift array
			}
			activePowerUpCount--; // Decrement active power-up count
			i--; // Adjust index since we removed an item
		}
		if (CheckPowerUpCollection(powerUpArray[i])) {
			if (powerUpArray[i].type == DOUBLESCORE) {
				doubleScoreFlag = true;
			}
			else
			{
				slowDownTimeFlag = true;
			}
			for (int j = i; j < activePowerUpCount - 1; j++) {
				powerUpArray[j] = powerUpArray[j + 1]; // Shift array
			}
			activePowerUpCount--; // Decrement active power-up count
			i--; //
		}
	}
}

void updateCollectiblePosition(float deltaTime) {
	for (int i = 0; i < activeCollectibleCount; i++) {
		if (!slowDownTimeFlag) {
			collectibleArray[i].x -= collectibleArray[i].speed * deltaTime;
		}
		else
		{
			collectibleArray[i].x -= slowedDownGameSpeedMultiplier * deltaTime;
		}
		if (collectibleArray[i].x < 0) { //check if metoer is off screen
			for (int j = i; j < activeCollectibleCount - 1; j++) {
				collectibleArray[j] = collectibleArray[j + 1];//shift meteor array to remove meteor 
			}
			activeCollectibleCount--; //decrement actie metoer count
			i--; //adjust i since we removed offscreen meteor

		}
		if (CheckPickup(collectibleArray[i])) {
			if (doubleScoreFlag) {
				score+= 2;
				
			}
			else
			{
				score++;
			}
			
			// Remove the collected collectible
			for (int j = i; j < activeCollectibleCount - 1; j++) {
				collectibleArray[j] = collectibleArray[j + 1];
			}
			activeCollectibleCount--;
			i--;  // Adjust the index after removing the collectible
		
		}

	}
}

void updateObstaclePosition(float deltaTime) {
	//loop through active meteors
	for (int i = 0; i < activeMeteorCount; i++) {
		if (!slowDownTimeFlag) {
			meteorsArray[i].x -= meteorsArray[i].speed * deltaTime; //move left
		}
		else
		{
			meteorsArray[i].x -= slowedDownGameSpeedMultiplier * deltaTime;
		}
		if (meteorsArray[i].x < 0) { //check if metoer is off screen
			for (int j = i; j < activeMeteorCount - 1; j++) {
				meteorsArray[j] = meteorsArray[j + 1];//shift meteor array to remove meteor 
			}
			activeMeteorCount--; //decrement actie metoer count
			i--; //adjust i since we removed offscreen meteor

		}
		if (!invincibleFlag && CheckCollision( meteorsArray[i])) {
			health--;
			invincibleFlag = true;
			invincibleTimer = invincibleDuration;
		}
	}

}




void DrawMan(float x, float y) {
	if (blinkingFlag && invincibleFlag) {
		return;
	}
	if (isDucking) {
		// Ducking state (shortened body and legs)
		// Head (same size but lower due to ducking)
		glColor3f(1.0f, 1.0f, 1.0f);  // Skin tone color
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float theta = i * M_PI / 180;
			glVertex2f(x + 15 * cos(theta), y + 15 * sin(theta) + 30); // Bigger head (radius 15)
		}
		glEnd();

		// Shortened body (wider and shorter)
		glColor3f(1.0f, 1.0f, 1.0f);  // Blue shirt
		glBegin(GL_QUADS);
		glVertex2f(x + 20, y + 25);  // Wide body
		glVertex2f(x - 20, y + 25);
		glVertex2f(x - 15, y);
		glVertex2f(x + 15, y);
		glEnd();

		// Arms (shortened)
		  // Same as body color
		glLineWidth(8.0f);  // Thicker arms
		glBegin(GL_LINES);
		glVertex2f(x + 20, y + 22);  // Right arm (shorter)
		glVertex2f(x + 35, y + 10);
		glVertex2f(x - 20, y + 22);  // Left arm (shorter)
		glVertex2f(x - 35, y + 10);
		glEnd();

		// Shortened legs
		  // Green pants
		glBegin(GL_QUADS);
		glVertex2f(x + 10, y);   // Right leg
		glVertex2f(x + 5, y);
		glVertex2f(x + 5, y - 20);
		glVertex2f(x + 10, y - 20);
		glEnd();

		glBegin(GL_QUADS);
		glVertex2f(x - 10, y);   // Left leg
		glVertex2f(x - 5, y);
		glVertex2f(x - 5, y - 20);
		glVertex2f(x - 10, y - 20);
		glEnd();
	}
	else {
		// Adjusted Standing state for a height of 120 units.

		// Head (slightly larger, circular head)
		glColor3f(1.0f, 0.85f, 0.7f);  // Skin tone color
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float theta = i * M_PI / 180;
			glVertex2f(x + 18 * cos(theta), y + 18 * sin(theta) + 65); // Bigger head (radius 18)
		}
		glEnd();

		// Body (taller, wider)
		// Blue shirt
		glBegin(GL_QUADS);
		glVertex2f(x + 25, y + 60);  // Taller and wider body
		glVertex2f(x - 25, y + 60);
		glVertex2f(x - 20, y + 30);
		glVertex2f(x + 20, y + 30);
		glEnd();

		// Arms (longer, thicker)
		glLineWidth(8.0f);  // Thicker arms
		glBegin(GL_LINES);
		glVertex2f(x + 25, y + 55);  // Right arm
		glVertex2f(x + 50, y + 30);
		glVertex2f(x - 25, y + 55);  // Left arm
		glVertex2f(x - 50, y + 30);
		glEnd();

		// Legs (longer, proportional to new height)
		// Green pants
		glBegin(GL_QUADS);
		glVertex2f(x + 12, y + 30);   // Right leg
		glVertex2f(x + 7, y + 30);
		glVertex2f(x + 7, y - 30);    // Longer legs
		glVertex2f(x + 12, y - 30);
		glEnd();

		glBegin(GL_QUADS);
		glVertex2f(x - 12, y + 30);   // Left leg
		glVertex2f(x - 7, y + 30);
		glVertex2f(x - 7, y - 30);    // Longer legs
		glVertex2f(x - 12, y - 30);
		glEnd();
	}
}

void DrawSimpleMeteor(float x, float y, float radius) {
	int numSegments = 8;  // Fewer segments for a rougher look
	float angle;

	// Base color of the meteor
	if (!slowDownTimeFlag) {
		glColor3f(0.6f, 0.5f, 0.5f);  // Grayish-brown
	}
	else
	{
		glColor3f(0, 0.5, 0.5);
	}

	// Draw the rough circular base of the meteor
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);  // Center of the meteor

	for (int i = 0; i <= numSegments; i++) {
		angle = i * 2.0f * M_PI / numSegments;
		float offsetX = radius * cos(angle);
		float offsetY = radius * sin(angle);
		glVertex2f(x + offsetX, y + offsetY);
	}

	glEnd();

	// Add a few jagged polygons for an irregular shape
	glColor3f(0.5f, 0.4f, 0.4f);  // Darker shade for jagged areas
	glBegin(GL_POLYGON);
	glVertex2f(x + radius * 0.8f, y + radius * 0.2f);
	glVertex2f(x + radius * 0.9f, y - radius * 0.1f);
	glVertex2f(x + radius * 0.5f, y - radius * 0.5f);
	glVertex2f(x + radius * 0.2f, y - radius * 0.2f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x - radius * 0.7f, y + radius * 0.3f);
	glVertex2f(x - radius * 0.9f, y - radius * 0.1f);
	glVertex2f(x - radius * 0.4f, y - radius * 0.3f);
	glVertex2f(x - radius * 0.2f, y + radius * 0.2f);
	glEnd();


	// Draw the tail of the meteor with varying lengths, colors, and start points
	glBegin(GL_LINES);

	// Tail line 1 - Slightly above, shorter length, red
	glColor3f(1.0f, 0.2f, 0.2f);  // Red color for the top tail
	glVertex2f(x + radius * 0.6f, y + radius * 0.2f);  // Start from slightly above the center on the right side
	glVertex2f(x + radius * 2.0f, y + 0.4f);           // Extend with shorter length

	// Tail line 2 - Middle, longer length, orange
	glColor3f(1.0f, 0.5f, 0.2f);  // Orange color for the middle tail
	glVertex2f(x + radius * 0.8f, y);                  // Start from the middle on the right side
	glVertex2f(x + radius * 2.5f, y);                  // Extend with medium length

	// Tail line 3 - Slightly below, longest length, yellow
	glColor3f(1.0f, 1.0f, 0.2f);  // Yellow color for the bottom tail
	glVertex2f(x + radius * 0.6f, y - radius * 0.2f);  // Start from slightly below the center on the right side
	glVertex2f(x + radius * 3.0f, y - 0.3f);           // Extend with longest length

	glEnd();

}

void DrawCollectibleGem(float x, float y) {



	//big frame 
	if (shineToggle) {
		glColor3f(.0f, 1.0f, 1.0f);
	}
	else
	{
		glColor3f(1.0f, 0.4f, 0.2f);
	}
	
	glBegin(GL_QUADS);
	glVertex2f(x, y + 20);
	glVertex2f(x + 10, y + 10);
	glVertex2f(x, y);
	glVertex2f(x - 10, y + 10);
	glEnd();



	//small  frame 
	if (shineToggle) {
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	else
	{
		glColor3f(1.0f, 0.7f, 0.2f);
	}
	glBegin(GL_QUADS);
	glVertex2f(x, y + 18);
	glVertex2f(x + 8, y + 10);
	glVertex2f(x, y);
	glVertex2f(x - 8, y + 10);
	glEnd();

	//shiny triangle in middle
	if (shineToggle) {
		glColor3f(1.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	glBegin(GL_QUADS);
	glVertex2f(x, y + 18);
	glVertex2f(x + 6, y + 10);
	glVertex2f(x, y);
	glVertex2f(x + 2, y + 10);
	glEnd();

	//small triangle in the upper left 
	if (shineToggle) {
		glColor3f(1.0f, 1.0f, 0.0f);
		
	}
	else
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	glBegin(GL_QUADS);
	glVertex2f(x, y + 18);
	glVertex2f(x - 4, y + 10);
	glVertex2f(x, y);
	glVertex2f(x - 2, y + 10);
	glEnd();

	//bottom right big triangle
	if (shineToggle) {
		glColor3f(1.0f, 0.4f, 0.2f);
	}
	else
	{
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glBegin(GL_TRIANGLES);
	glVertex2f(x + 8, y + 10);
	glVertex2f(x, y);
	glVertex2f(x - 2, y + 10);
	glEnd();

	//lower left triangle
		if (shineToggle) {
		glColor3f(1.0f, 1.0f, 0.0f);
		
	}
	else
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	//glColor3f(0.0f, 0.7f, 0.2f);
	glBegin(GL_TRIANGLES);
	glVertex2f(x - 8, y + 10);
	glVertex2f(x, y);
	glVertex2f(x - 2, y + 10);
	glEnd();

}

void DrawClockPowerUp(float x, float y) {
	// Outer Circle (Clock Face) - Larger and darker
	glColor3f(0.2f, 0.2f, 0.2f); // Darker gray color for the clock face
	glBegin(GL_POLYGON);
	// 32 points to approximate a larger outer circle (30 units radius)
	for (int i = 0; i < 32; ++i) {
		float angle = 2.0f * M_PI * i / 32;
		float x_offset = 30.0f * cos(angle);
		float y_offset = 30.0f * sin(angle);
		glVertex2f(x + x_offset, y + y_offset);
	}
	glEnd();

	// Inner Circle (Center of the Clock) - Slightly larger and light gray
	glColor3f(0.8f, 0.8f, 0.8f); // Lighter gray for the center
	glBegin(GL_POLYGON);
	// 32 points to approximate the inner circle (20 units radius)
	for (int i = 0; i < 32; ++i) {
		float angle = 2.0f * M_PI * i / 32;
		float x_offset = 20.0f * cos(angle);
		float y_offset = 20.0f * sin(angle);
		glVertex2f(x + x_offset, y + y_offset);
	}
	glEnd();

	// Hour Hand (Animated)
	glColor3f(1.0f, 0.0f, 0.0f); // Red for the hour hand
	glLineWidth(3.0f); // Make the hour hand thicker
	// Apply Rotation for hour hand
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(hourHandAngle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);           // Start from the center of the clock
	glVertex2f(0.0f, 20.0f);   // Extend the hand upwards (20 units)
	glEnd();
	glPopMatrix();

	// Minute Hand (Animated)
	glColor3f(0.0f, 0.0f, 1.0f); // Blue for the minute hand
	glLineWidth(3.0f);     
	
	// Apply rotation for the minute hand
	glPushMatrix();
	glTranslatef(x, y, 0.0f);  // Move to the center of the clock
	glRotatef(minuteHandAngle, 0.0f, 0.0f, 1.0f); // Rotate by the current angle
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);        // Start from the center of the clock
	glVertex2f(15.0f, 0.0f);       // Extend the hand rightwards (15 units)
	glEnd();
	glPopMatrix();

	// Restore default line width
	glLineWidth(1.0f);
}


void DrawLightningBoltPowerUp(float x, float y) {
	// Draw the filled orange circle as the background
	glColor3f(1.0f, 1.0f, 1.0f); // white color for the outline
	glBegin(GL_POLYGON);
	// 32 points to approximate a larger outer circle (30 units radius)
	for (int i = 0; i < 32; ++i) {
		float angle = 2.0f * M_PI * i / 32;
		float x_offset = 30.0f * borderScale *cos(angle);
		float y_offset = 30.0f * borderScale  * sin(angle);
		glVertex2f(x + x_offset, y + y_offset);
	}
	glEnd();

	// Draw the outline using a triangle fan
	glColor3f(1.0f, 0.5f, 0.0f); // Orange color for the background
	glBegin(GL_POLYGON);
	// 32 points to approximate the inner circle (20 units radius)
	for (int i = 0; i < 32; ++i) {
		float angle = 2.0f * M_PI * i / 32;
		float x_offset = 20.0f * cos(angle);
		float y_offset = 20.0f * sin(angle);
		glVertex2f(x + x_offset, y + y_offset);
	}
	glEnd();

	// Draw the lightning bolt using lines
	glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the lightning bolt body
	glLineWidth(4.0f); // Thicker lines for better visibility

	// Draw the lightning bolt using lines, moved down by 4 pixels
	glBegin(GL_LINES);
	glVertex2f(x, y - 9);              // Starting point at the bottom (moved down by 4 pixels)
	glVertex2f(x + 10, y + 9);         // Upper right point (moved down by 4 pixels)
	glVertex2f(x + 10, y + 9);         // Upper right point
	glVertex2f(x + 3, y + 3);          // Mid right point (moved down by 4 pixels)
	glVertex2f(x + 3, y + 3);          // Mid right point
	glVertex2f(x + 7, y + 13);         // Upper mid point (moved down by 4 pixels)
	glVertex2f(x + 7, y + 13);         // Upper mid point
	glVertex2f(x - 5, y + 6);          // Mid left point (moved down by 4 pixels)
	glVertex2f(x - 5, y + 6);          // Mid left point
	glVertex2f(x + 5, y + 6);          // Mid right point (moved down by 4 pixels)
	glVertex2f(x + 5, y + 6);          // Mid right point
	glVertex2f(x, y - 9);              // Connecting back to the bottom (moved down by 4 pixels)
	glEnd();

	glLineWidth(1.0f); // Restore default line width
}





void DrawPowerUp(float x, float y, PowerUpType type) {
	if (type == DOUBLESCORE) {
		DrawLightningBoltPowerUp(x, y);
	}
	else
	{
		DrawClockPowerUp(x, y);
	}
}

void DrawHearts(float x, float y) {
	float scale = 2.0f;

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y);
	glVertex2f(x - 4 *scale, y + 5 *scale);
	glVertex2f(x + 4 * scale, y + 5 * scale);
	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(x - 4 * scale, y + 5 * scale);
	glVertex2f(x - 3 * scale, y + 8 * scale);
	glVertex2f(x - 1 * scale, y + 8 * scale);
	glVertex2f(x, y + 5 * scale);
	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(x + 4 * scale, y + 5 * scale);
	glVertex2f(x + 3 * scale, y + 8 * scale);
	glVertex2f(x + 1 * scale, y + 8 * scale);
	glVertex2f(x, y + 5 * scale);
	glEnd();

}

void DrawUpperBoundary(float screenWidth, float screenHeight) {
	// Draw the upper boundary background (space)
	glColor3f(0.0f, 0.0f, 0.2f); // Dark navy blue for the space background
	glBegin(GL_QUADS);
	glVertex2f(0, screenHeight);         // Top-left
	glVertex2f(screenWidth, screenHeight); // Top-right
	glVertex2f(screenWidth, 0);          // Bottom-right
	glVertex2f(0, 0);                     // Bottom-left
	glEnd();

	// Add stars in the upper boundary
	glColor3f(1.0f, 1.0f, 1.0f); // White for stars
	for (int i = 0; i < 100; ++i) { // Increased star count for more visibility
		float starX = rand() % (int)screenWidth;
		float starY = rand() % (int)(screenHeight - 50); // Ensure stars are within the upper boundary

		glBegin(GL_POINTS);
		glVertex2f(starX, starY); // Place stars randomly
		glEnd();
	}

	// Add two planets with fixed positions and sizes
	glColor3f(1.0f, 1.0f, 0.0f); // Yellowish color for planets

	// Planet 1 (Fixed position and size)
	float planet1X = 150.0f;  // Fixed X position for planet 1
	float planet1Y = 400.0f;  // Fixed Y position for planet 1
	float planet1Size = 40.0f;  // Fixed size for planet 1

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(planet1X, planet1Y); // Center of planet 1
	for (int j = 0; j <= 30; ++j) {
		float angle = 2.0f * M_PI * j / 30;
		glVertex2f(planet1X + cos(angle) * planet1Size, planet1Y + sin(angle) * planet1Size);
	}
	glEnd();

	// Planet 2 (Fixed position and size)
	float planet2X = 500.0f;  // Fixed X position for planet 2
	float planet2Y = 250.0f;  // Fixed Y position for planet 2
	float planet2Size = 50.0f;  // Fixed size for planet 2

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(planet2X, planet2Y); // Center of planet 2
	for (int j = 0; j <= 30; ++j) {
		float angle = 2.0f * M_PI * j / 30;
		glVertex2f(planet2X + cos(angle) * planet2Size, planet2Y + sin(angle) * planet2Size);
	}
	glEnd();

}

void DrawLowerBoundary(float screenWidth, float screenHeight) {
	// Draw the lower boundary surface (moon-like surface)
	glColor3f(0.5f, 0.5f, 0.5f); // Grey color for the moon's surface
	glBegin(GL_QUADS);
	glVertex2f(0, 0);            // Bottom-left
	glVertex2f(screenWidth, 0);  // Bottom-right
	glVertex2f(screenWidth, 100); // Top-right (100 units tall)
	glVertex2f(0, 100);           // Top-left
	glEnd();

	// Add static craters on the moon's surface
	float craterPositions[3][2] = {
		{200, 50},  // Crater 1 position (X, Y)
		{400, 30},  // Crater 2 position (X, Y)
		{600, 70}   // Crater 3 position (X, Y)
	};
	float craterSizes[3] = { 25, 20, 30 }; // Crater sizes (radius)

	for (int i = 0; i < 3; ++i) {
		float craterX = craterPositions[i][0];
		float craterY = craterPositions[i][1];
		float craterRadius = craterSizes[i];

		// Draw crater as a circle
		glColor3f(0.3f, 0.3f, 0.3f); // Darker grey for the crater
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(craterX, craterY); // Center of the crater
		for (int j = 0; j <= 30; ++j) {
			float angle = 2.0f * M_PI * j / 30;
			glVertex2f(craterX + cos(angle) * craterRadius, craterY + sin(angle) * craterRadius);
		}
		glEnd();
	}

	// Add slight texture or ridges on the surface using lines
	glColor3f(0.4f, 0.4f, 0.4f); // Slightly darker grey for the ridges
	glBegin(GL_LINES);
	for (int i = 0; i < screenWidth; i += 50) {
		glVertex2f(i, 100);  // Top of the boundary
		glVertex2f(i + 25, 80);  // Ridge slanting downward
	}
	glEnd();

	// Draw additional decorative details (small rocks)
	glColor3f(0.35f, 0.35f, 0.35f); // Color for small rocks
	float rockPositions[4][2] = {
		{150, 20}, {350, 40}, {550, 60}, {750, 25}
	};
	float rockSizes[4] = { 8, 12, 10, 9 }; // Sizes of the rocks

	for (int i = 0; i < 4; ++i) {
		float rockX = rockPositions[i][0];
		float rockY = rockPositions[i][1];
		float rockRadius = rockSizes[i];

		// Draw rock as a circle
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(rockX, rockY); // Center of the rock
		for (int j = 0; j <= 30; ++j) {
			float angle = 2.0f * M_PI * j / 30;
			glVertex2f(rockX + cos(angle) * rockRadius, rockY + sin(angle) * rockRadius);
		}
		glEnd();
	}
}


void jump() {

	if (!isJumping) {
		isJumping = true;
		jumpVelocity = 12.0f;
	}

	}

void updatePlayerPosition() {
	if (isJumping && !isDucking) {
		playerY += jumpVelocity;
		jumpVelocity += gravity; //apply gravity to rach max point and fall down

		if (playerY <= jumpStartY) {
			playerY = jumpStartY;
			isJumping = false;
		}
	} if (isJumping && isDucking) {
		playerY += jumpVelocity;
		jumpVelocity += gravity * 3;

		if (playerY <= jumpStartY) {
			playerY = jumpStartY;
			isJumping = false;
			isDucking = false;
		}
	}
}



void updateScore() {
	if (!doubleScoreFlag) {
		glColor3f(1, 1, 1);
	}
	else
	{
		glColor3f(1, 1, 0);
	}
	
	char* scoreString[20];
	sprintf((char*)scoreString, "Score:%02d", score);
	print(screenWidth / 2, 550, (char*)scoreString);

}

void updateGameTime(float deltaTime) {
	if (!isGameOver && !isGameEnd) {
		gameTime += deltaTime;
		if (int(gameTime) % 10 == 0) {
			spawnRate -= 0.05f;
			gameSpeedMultiplier+= 5.0f;
			if (spawnRate < 0.7f) spawnRate = 0.7f;
		}
	}
	//convert gametime to string format
	int minutes = static_cast<int> (gameTime) / 60;
	int seconds = static_cast<int> (gameTime) % 60;

	if (minutes == 1) {
		isGameEnd = true;
	}

	glColor3f(1, 1, 1);	
	char* timeString[20];
	sprintf((char*)timeString, "Time: %02d:%02d", minutes, seconds);
	print(screenWidth - 150, 550, (char*)timeString);
	
}



void Update(int Value) {
	float deltaTime = 0.016; //time between current and previous frame to make it 60 fps
	
	if (!isGameOver && !isGameEnd) {
		
		updateObstaclePosition(deltaTime);
		updateCollectiblePosition(deltaTime);
		updatePowerUpPosition(deltaTime);
		if (invincibleFlag) {
			invincibleTimer -= deltaTime;
			blinkingTimer += deltaTime;
			

			//toggle blinking every 0.1 seconds
			if (blinkingTimer >= 0.1f) {
				blinkingFlag = !blinkingFlag; //toggle
				blinkingTimer = 0.0f; //reset timer
			}

			//if invincible duration is up reset flags to false
			if (invincibleTimer <= 0.0f) {
				invincibleFlag = false;
				blinkingFlag = false;
			}

		}

		if (doubleScoreFlag) {
			doubleScoreTimer += deltaTime;
			if (doubleScoreTimer > 5.0f) { //powerup lasts 5 seconds
				doubleScoreTimer = 0.0f; //reset timer
				doubleScoreFlag = false; //reset flag 
			}
		}
		if (slowDownTimeFlag) {
			slowDownTimer += deltaTime;
			if (slowDownTimer > 5.0f) {
				slowDownTimer = 0.0f; //reset timer
				slowDownTimeFlag = false; //reset flag
				
			}
		}


		updatePlayerPosition();

		checkGameOver();

		
		static float collectibleSpawnTimer = 0.0f;
		static float meteorSpawnTimer = 0.0f;
		static float powerUpSpawnTimer = 0.0f;
		meteorSpawnTimer += deltaTime;
		collectibleSpawnTimer += deltaTime;
		powerUpSpawnTimer += deltaTime;
		gemShineTimer += deltaTime; 


		if (gemShineTimer >= shineDuration) {
			shineToggle = !shineToggle;
			gemShineTimer = 0.0f;
		}
		// Update minute hand angle
		hourHandAngle += 180 * deltaTime;
		minuteHandAngle += 90.0f * deltaTime; // Rotate the hand by 45 degrees per second

		// Ensure the angle doesn't exceed 360 degrees
		if (minuteHandAngle >= 360.0f) {
			minuteHandAngle -= 360.0f;
		}

		if (borderGrowing) {
			borderScale += borderPulseSpeed; // Increase size
			if (borderScale >= 1.1f) borderGrowing = false; // Reverse direction
		}
		else {
			borderScale -= borderPulseSpeed; // Decrease size
			if (borderScale <= 1.0f) borderGrowing = true; // Reverse direction
		}

		if (powerUpSpawnTimer >= powerUpSpawnRate) {
			spawnPowerUP();
			powerUpSpawnTimer = 0.0f;
		}
		

		if (!slowDownTimeFlag) {
			// Spawn collectibles at defined intervals
			if (collectibleSpawnTimer >= CollectibleSpawnRate) {
				spawnCollectible(); // Spawn a collectible
				collectibleSpawnTimer = 0.0f; // Reset the timer
			}

			// Spawn meteors at defined intervals
			if (meteorSpawnTimer >= spawnRate) {
				spawnMeteor(); // Spawn a meteor
				meteorSpawnTimer = 0.0f; // Reset the timer
			}
		}
		else
		{
			// Spawn collectibles at defined intervals
			if (collectibleSpawnTimer >= CollectibleSpawnRate *3) {
				spawnCollectible(); // Spawn a collectible
				collectibleSpawnTimer = 0.0f; // Reset the timer
			}

			// Spawn meteors at defined intervals
			if (meteorSpawnTimer >= spawnRate * 3) {
				spawnMeteor(); // Spawn a meteor
				meteorSpawnTimer = 0.0f; // Reset the timer
			}
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, Update, 0);
}




void Display() {
	float deltaTime = 0.016;
	glClear(GL_COLOR_BUFFER_BIT);

	DrawUpperBoundary(screenWidth, screenHeight);
	DrawLowerBoundary(screenWidth, screenHeight);

	updateGameTime(deltaTime);
	updateScore();
	

	for (int i = 0; i < health; i++) {
		DrawHearts(10 + i*20, 570);
	}

	

	DrawMan(playerX,playerY);
	



	for (int i = 0; i < activePowerUpCount; i++) {
		DrawPowerUp(powerUpArray[i].x, powerUpArray[i].y, powerUpArray[i].type);
	}


	for (int i = 0; i < activeMeteorCount; i++) {
		DrawSimpleMeteor(meteorsArray[i].x, meteorsArray[i].y, meteorsArray[i].radius);
		
	}
	for (int i = 0; i < activeCollectibleCount; i++) {
		DrawCollectibleGem(collectibleArray[i].x, collectibleArray[i].y);
		DrawCollectibleGem(collectibleArray[i+1].x - 15, collectibleArray[i].y);
	
	}

	if (isGameOver) {
		drawGameOver();
	}
	if (isGameEnd) {
		drawGameEnd();
	}

	glutSwapBuffers();




	glFlush();
}


void KeyboardInput(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		jump();
		break;
	case 's':
		if (!isDucking) {
			isDucking = true; //start ducking
		}
		break;
	case ' ':
		//start game
		break;
	}

	glutPostRedisplay(); //request to redraw the scene

}


void handleKeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 's':  // Stop ducking
		isDucking = false;
		break;
	}
}



void main(int argc, char** argr) {
	//initialize GLUT
	glutInit(&argc, argr);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight); //size 800x600
	glutInitWindowPosition(150, 150);
	glutCreateWindow("Mariolike");

	//set up display and input callback function
	glutDisplayFunc(Display);
	glutKeyboardFunc(KeyboardInput);
	glutKeyboardUpFunc(handleKeyboardUp);
	glutTimerFunc(16, Update, 0); // Call every 16ms
	
	//set up background color and orthographic projection
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0,screenWidth, screenHeight);  // x y width height
	gluOrtho2D(0, screenWidth, 0, screenHeight);

	//enter main loop
	glutMainLoop();
}



