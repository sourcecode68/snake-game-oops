#pragma once
#include <raylib.h>

/**
 * =============================
 * Class Overview
 * =============================
 * The **Button** class represents a clickable UI element rendered using a
 * texture. It supports loading an image, scaling it, drawing it, and detecting
 * mouse click interactions.
 *
 * =============================
 * Member Variables (private)
 * =============================
 * - **Texture2D texture** : Stores the image used for the visual appearance of the button.
 * - **Vector2 position**  : Screen coordinates where the button is drawn.
 *
 * =============================
 * Member Functions (public)
 * =============================
 * **Button(const char *imagePath, Vector2 imagePosition, float scale)**
 *   - Objective: Load and scale a button image, convert it into a usable texture,
 *                and set its drawing position.
 *   - Input: imagePath → File path of the image.
 *            imagePosition → Screen coordinates.
 *            scale → Scale factor.
 *   - Output: Initializes internal texture and position.
 *   - Side Effects: Allocates GPU memory when loading texture.
 *
 * **~Button()**
 *   - Objective: Free GPU memory used by the texture.
 *   - Side Effects: If texture is not unloaded, memory leaks occur.
 *
 * **void Draw()**
 *   - Objective: Render the button on the screen.
 *   - Side Effects: Requires active BeginDrawing/EndDrawing block.
 *
 * **bool isPressed(Vector2 mousePos, bool mousePressed)**
 *   - Objective: Determine whether the button was clicked.
 *   - Input: mouse position + click state.
 *   - Return: true if click is inside button bounds.
 */
class Button
{
public:
    Button(const char *imagePath, Vector2 imagePosition, float scale);
    ~Button();
    void Draw();
    bool isPressed(Vector2 mousePos, bool mousePressed);

private:
    Texture2D texture; ///< Texture used to visually represent the button.
    Vector2 position;  ///< Screen coordinates where the button is drawn.
};