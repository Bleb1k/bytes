#include <raylib.h>

int main()
{
	int w = 700, h = 550;
	InitWindow(w, h, "raylib [core] example - basic window");
	SetWindowMinSize(200, 100);
	SetWindowPosition(1200, 200);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	RenderTexture2D target = LoadRenderTexture(w, h);

	Shader shader = LoadShader(0, "src/grayscale.fs");

	int testUniLoc = GetShaderLocation(shader, "test");
	int resLoc = GetShaderLocation(shader, "res");
	int viewPosLoc = GetShaderLocation(shader, "viewPos");
	int viewTargetLoc = GetShaderLocation(shader, "viewTarget");
	int runTimeLoc = GetShaderLocation(shader, "runTime");

	int test[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	SetShaderValueV(shader, testUniLoc, test, SHADER_UNIFORM_INT, 8);
	SetShaderValue(shader, resLoc, &(Vector2){w, h}, SHADER_UNIFORM_VEC2);

	Camera camera = {
			.position = (Vector3){2.5f, 2.5f, 3.0f}, // Camera position
			.target = (Vector3){0.0f, 0.5f, 0.0f},	 // Camera looking at point
			.up = (Vector3){0.0f, 1.0f, 0.0f},			 // Camera up vector (rotation towards target)
			.fovy = 70.0f,													 // Camera field-of-view Y
			.projection = CAMERA_PERSPECTIVE				 // Camera projection type
	};

	float runTime = 0.0f;

	while (!WindowShouldClose())
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsCursorHidden())
		{
			EnableCursor();
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			DisableCursor();
		}
		// Exit if unfocused
		//----------------------------------------------------------------------------------
		if (!IsWindowFocused())
			break;

		// Update
		//----------------------------------------------------------------------------------
		UpdateCamera(&camera, CAMERA_FREE|CAMERA_FIRST_PERSON);
		// Vector2 md = GetMouseDelta();
		// Vector3 vel = {0, 0, 0};
		// if (IsKeyDown(KEY_W))
		// 	vel.x = 1;
		// if (IsKeyDown(KEY_S))
		// 	vel.x = -1;
		// if (IsKeyDown(KEY_A))
		// 	vel.z = 1;
		// if (IsKeyDown(KEY_D))
		// 	vel.z = -1;
		// UpdateCameraPro(&camera, (Vector3){0, 0, 0}, (Vector3){md.x, md.y, 0}, 0);

		// UpdateCameraPro

		float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
		float cameraTarget[3] = {camera.target.x, camera.target.y, camera.target.z};
		float deltaTime = GetFrameTime();
		runTime += deltaTime;

		SetShaderValue(shader, viewPosLoc, cameraPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, viewTargetLoc, cameraTarget, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, runTimeLoc, &runTime, SHADER_UNIFORM_FLOAT);

		// Resize
		//----------------------------------------------------------------------------------
		if (IsWindowResized())
		{
			w = GetScreenWidth(), h = GetScreenHeight();
			UnloadRenderTexture(target);			// Unload old render texture
			target = LoadRenderTexture(w, h); // Re-create render texture
			SetShaderValue(shader, resLoc, &(Vector2){w, h}, SHADER_UNIFORM_VEC2);
		}

		// Draw
		//----------------------------------------------------------------------------------
		// Needed for shaders depending on coordinates of texture (e.g. from 0.0 to 1.0)
		// BeginTextureMode(target); // Enable drawing to texture
		// {
		// 	ClearBackground(BLACK);
		// 	// DrawRectangle(0, 0, w, h, BLACK); // Is this necessary?
		// }
		// EndTextureMode(); // End drawing to texture (now we have a blank texture available for the shader)

		BeginDrawing();
		{
			ClearBackground(RAYWHITE); // Clear screen background

			// NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
			BeginShaderMode(shader);
			{
				// DrawRectangle(0, 0, w, h, PINK);
				DrawRectangle(0, 0, w, h, WHITE); // (target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)target.texture.height}, (Vector2){0.0f, 0.0f}, WHITE);
			}
			EndShaderMode();

			DrawText(TextFormat("Fps: (%d)", GetFPS()), 0, 0, 20, PURPLE);
			DrawText(TextFormat("Camera Pos: (x:%2.2f,y:%2.2f,z:%2.2f)", camera.position.x, camera.position.y, camera.position.z), 0, 30, 20, PURPLE);
			DrawText(TextFormat("Camera Trg: (x:%2.2f,y:%2.2f,z:%2.2f)", camera.target.x, camera.target.y, camera.target.z), 0, 60, 20, PURPLE);
		}
		EndDrawing();
	}

	UnloadShader(shader); // Unload shader
	CloseWindow();

	return 0;
}
