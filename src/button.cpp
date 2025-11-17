#include "button.hpp"

/**
 * Button::Button
 * ============================
 * Objective:
 *   Load an image from disk, scale it, convert it into a texture, and initialize
 *   the button's on-screen position.
 *
 * Input:
 *   - const char* imagePath → File path of the button image.
 *   - Vector2 imagePosition → Screen coordinates where the button will appear.
 *   - float scale → Scaling factor applied to the image before converting to a texture.
 *
 * Output:
 *   - Loads and assigns a scaled texture to the button.
 *   - Stores the drawing position.
 *
 * Return Value:
 *   - None (constructor).
 *
 * Side Effects:
 *   - Loads image and texture into system/GPU memory.
 *   - If file path is invalid, texture may fail to load and cause runtime issues.
 */
Button::Button(const char *imagePath, Vector2 imagePosition, float scale)
{
    // Load the raw image from the provided file path (stored in RAM).
    Image image = LoadImage(imagePath);

    // Store the original width and height of the image.
    int originalWidth = image.width;
    int originalHeight = image.height;

    // Calculate scaled dimensions using the given scale factor.
    int newWidth = static_cast<int>(originalWidth * scale);
    int newHeight = static_cast<int>(originalHeight * scale);

    // Resize the image in RAM before converting it to a texture.
    ImageResize(&image, newWidth, newHeight);

    // Upload the processed image to VRAM as a texture.
    texture = LoadTextureFromImage(image);

    // Free image data from RAM—texture is now safely stored in GPU memory.
    UnloadImage(image);

    // Store the final drawing position of the button.
    position = imagePosition;
}

/**
 * Button::~Button
 * =============================
 * Objective:
 *   Properly unload the texture from GPU memory.
 *
 * Input: None
 * Output: Frees GPU texture resources.
 * Return Value: None (destructor)
 *
 * Side Effects:
 *   - If texture unloading is skipped, program leaks GPU memory.
 */
Button::~Button()
{
    UnloadTexture(texture); // Release VRAM used by the texture.
}

/**
 * Button::Draw
 * =============================
 * Objective:
 *   Render the button texture at its assigned position.
 *
 * Input: None
 * Output: Draws texture onto the active render target.
 * Return Value: void
 *
 * Side Effects:
 *   - Requires a valid BeginDrawing()/EndDrawing() block.
 *   - Uses GPU for rendering.
 *
 * Approach:
 *   Use DrawTextureV to draw texture at a Vector2 location.
 */
void Button::Draw()
{
    DrawTextureV(texture, position, WHITE); // Draw at given position using original texture colors.
}

/**
 * Button::isPressed
 * =============================
 * Objective:
 *   Determine whether the left mouse button was pressed inside the button area.
 *
 * Input:
 *   - Vector2 mousePos → Current mouse coordinates.
 *   - bool mousePressed → True if mouse button was clicked this frame.
 *
 * Output:
 *   - Returns true if the button was clicked.
 *
 * Return Value:
 *   - bool → true if click happens inside the button bounds, false otherwise.
 *
 * Side Effects:
 *   - None (pure logic).
 *
 * Approach:
 *   Build a Rectangle matching button size, then use raylib collision check.
 */
bool Button::isPressed(Vector2 mousePos, bool mousePressed)
{
    // Create a bounding rectangle equal to button position and texture dimensions.
    Rectangle rect = {position.x, position.y,
                      static_cast<float>(texture.width),
                      static_cast<float>(texture.height)};

    // Check if mouse is inside rectangle AND click occurred.
    if (CheckCollisionPointRec(mousePos, rect) && mousePressed)
    {
        return true; // Button was pressed.
    }
    return false; // Default: not pressed.
}