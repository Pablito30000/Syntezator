#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <vector>
#include <irrKlang.h>
#include <unordered_map>

using namespace irrklang;
ISoundEngine* soundEngine = nullptr;

std::unordered_map<char, const char*> keySounds;    // Key-to-sound file association
std::unordered_map<char, char> keyMappings;         // User-defined key mappings

bool isKeyMappingActive = false;

struct Note
{
    ImVec2 pos;     // Position of the note
    ImVec2 size;    // Size of the note
    ImU32 color;    // Color of the note
    bool locked;    // Indicates if the note's size is locked
};

std::vector<Note> active_notes;

// Structure representing a piano key
struct PianoKey
{
    char key;       // Keyboard key corresponding to this piano key
    bool pressed;   // Is the key pressed
    bool is_white;  // Is the key white (true) or black (false)
    ImVec2 pos;     // Position of the key
    ImVec2 size;    // Size of the key
};

std::vector<PianoKey> keys; // Array of piano keys

// Initialize with default mappings
void InitializeKeyMappings() {
    keyMappings['1'] = '1';
    keyMappings['Q'] = 'Q';
    keyMappings['2'] = '2';
    keyMappings['W'] = 'W';
    keyMappings['3'] = '3';
    keyMappings['E'] = 'E';
    keyMappings['4'] = '4';
    keyMappings['R'] = 'R';
    keyMappings['5'] = '5';
    keyMappings['T'] = 'T';
    keyMappings['6'] = '6';
    keyMappings['Y'] = 'Y';
    keyMappings['7'] = '7';
    keyMappings['U'] = 'U';
    keyMappings['8'] = '8';
    keyMappings['I'] = 'I';
    keyMappings['9'] = '9';
    keyMappings['O'] = 'O';
    keyMappings['0'] = '0';
    keyMappings['P'] = 'P';
    keyMappings['A'] = 'A';
    keyMappings['S'] = 'S';
    keyMappings['D'] = 'D';
    keyMappings['F'] = 'F';
    keyMappings['G'] = 'G';
    keyMappings['H'] = 'H';
    keyMappings['J'] = 'J';
    keyMappings['K'] = 'K';
    keyMappings['L'] = 'L';
}

// Update key sounds based on the current key mappings
void UpdateKeySounds() {
    keySounds.clear();
    keySounds[keyMappings['1']] = "notes/C3.ogg";
    keySounds[keyMappings['Q']] = "notes/D3.ogg";
    keySounds[keyMappings['2']] = "notes/E3.ogg";
    keySounds[keyMappings['W']] = "notes/F3.ogg";
    keySounds[keyMappings['3']] = "notes/G3.ogg";
    keySounds[keyMappings['E']] = "notes/A3.ogg";
    keySounds[keyMappings['4']] = "notes/C4.ogg";
    keySounds[keyMappings['R']] = "notes/D4.ogg";
    keySounds[keyMappings['5']] = "notes/E4.ogg";
    keySounds[keyMappings['T']] = "notes/F4.ogg";
    keySounds[keyMappings['6']] = "notes/G4.ogg";
    keySounds[keyMappings['Y']] = "notes/A4.ogg";
    keySounds[keyMappings['7']] = "notes/C5.ogg";
    keySounds[keyMappings['U']] = "notes/D5.ogg";
    keySounds[keyMappings['8']] = "notes/E5.ogg";
    keySounds[keyMappings['I']] = "notes/F5.ogg";
    keySounds[keyMappings['9']] = "notes/G5.ogg";
    keySounds[keyMappings['O']] = "notes/A5.ogg";
    keySounds[keyMappings['0']] = "notes/C6.ogg";
    keySounds[keyMappings['P']] = "notes/D6.ogg";
}

void ShowKeyMappingWindow(bool* p_open)
{
    if (!ImGui::Begin("Change Key Mappings", p_open))
    {
        ImGui::End();
        return;
    }

    isKeyMappingActive = *p_open;

    ImGui::Text("Click on a note to change its key:");

    static char selectedNote = '\0';
    static bool openChangeKeyPopup = false;
    static char newKey = '\0';

    std::unordered_map<char, const char*> noteLabels = {
        {'1', "C3"}, {'Q', "D3"}, {'2', "E3"}, {'W', "F3"}, {'3', "G3"}, {'E', "A3"},
        {'4', "C4"}, {'R', "D4"}, {'5', "E4"}, {'T', "F4"}, {'6', "G4"}, {'Y', "A4"},
        {'7', "C5"}, {'U', "D5"}, {'8', "E5"}, {'I', "F5"}, {'9', "G5"}, {'O', "A5"},
        {'0', "C6"}, {'P', "D6"}
    };

    for (const auto& entry : keyMappings)
    {
        const char* noteLabel = noteLabels[entry.first];

        ImGui::Text("Note %s -> Key %c", noteLabel, entry.second);

        if (ImGui::IsItemClicked())
        {
            selectedNote = entry.first;
            openChangeKeyPopup = true;
        }
    }

    if (openChangeKeyPopup)
    {
        ImGui::OpenPopup("Change Key");
        openChangeKeyPopup = false;
    }

    if (ImGui::BeginPopupModal("Change Key", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter new key for note %s", noteLabels[selectedNote]);

        ImGui::InputText("New Key", &newKey, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase);

        if (ImGui::Button("Apply"))
        {
            keyMappings[selectedNote] = newKey; 
            UpdateKeySounds(); 
            ImGui::CloseCurrentPopup();  
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();  
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

// Load sound files for each key
void LoadKeySounds()
{
    keySounds['1'] = "notes/C3.ogg";
    keySounds['Q'] = "notes/D3.ogg";
    keySounds['2'] = "notes/E3.ogg";
    keySounds['W'] = "notes/F3.ogg";
    keySounds['3'] = "notes/G3.ogg";
    keySounds['E'] = "notes/A3.ogg";
    keySounds['4'] = "notes/C4.ogg";
    keySounds['R'] = "notes/D4.ogg";
    keySounds['5'] = "notes/E4.ogg";
    keySounds['T'] = "notes/F4.ogg";
    keySounds['6'] = "notes/G4.ogg";
    keySounds['Y'] = "notes/A4.ogg";
    keySounds['7'] = "notes/C5.ogg";
    keySounds['U'] = "notes/D5.ogg";
    keySounds['8'] = "notes/E5.ogg";
    keySounds['I'] = "notes/F5.ogg";
    keySounds['9'] = "notes/G5.ogg";
    keySounds['O'] = "notes/A5.ogg";
    keySounds['0'] = "notes/C6.ogg";
    keySounds['P'] = "notes/D6.ogg";
    // Add the key mappings
}

void UpdateNotes(float deltaTime)
{
    const float note_speed = 100.0f;            // Speed of the note

    for (auto& note : active_notes)
    {
        note.pos.y -= note_speed * deltaTime;   // Move the note upwards
    }

    // Remove notes that have moved off the top of the window
    active_notes.erase(
        std::remove_if(active_notes.begin(), active_notes.end(),
            [](const Note& note) { return note.pos.y + note.size.y < 0.0f; }),
        active_notes.end());
}

void DrawNotes()
{
    for (const auto& note : active_notes)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(note.pos, ImVec2(note.pos.x + note.size.x, note.pos.y + note.size.y), note.color);
    }
}

// Initialize the piano keys
void InitPianoKeys()
{
    const int num_white_keys = 36; // Number of white keys
    const int num_black_keys = 25; // Number of black keys

    const float white_key_width = 25.0f;    // Width of a white key
    const float black_key_width = 15.0f;    // Width of a black key
    const float white_key_height = 125.0f;  // Height of a white key
    const float black_key_height = 75.0f;   // Height of a black key
    const float spacing = 2.5f;             // Spacing between keys

    const ImVec2 start_pos = ImVec2(750.0f - (white_key_width + spacing) * num_white_keys * 0.5f, 500.0f);

    // White keymap, should correspond to a standard keyboard layout
    const char white_keymap[] = "1234567890";

    // Black keymap corresponding to the correct keys for a piano layout
    const char black_keymap[] = "QWERTYUIOP";

    // Create white keys
    for (int i = 0; i < num_white_keys; ++i)
    {
        PianoKey key;
        key.key = (i < sizeof(white_keymap)) ? white_keymap[i] : ' ';
        key.pressed = false;
        key.is_white = true;
        key.pos = ImVec2(start_pos.x + i * (white_key_width + spacing), start_pos.y);
        key.size = ImVec2(white_key_width, white_key_height);
        keys.push_back(key);
    }

    // Create black keys
    int black_key_index = 0;
    for (int i = 0; i < num_white_keys - 1; ++i)
    {
        // Black keys are between certain white keys
        if (i == 2 || i == 6 || i == 9 || i == 13 || i == 16 || i == 20 || i == 23 || i == 27 || i == 30 || i == 34)
        {
            continue; // Skip position where there's no black key
        }

        PianoKey key;
        key.key = (black_key_index < sizeof(black_keymap)) ? black_keymap[black_key_index++] : ' ';
        key.pressed = false;
        key.is_white = false;

        // Position of the black key is between the current and the next white key
        float black_key_x = start_pos.x + (i + 1) * (white_key_width + spacing) - (black_key_width * 0.5f);
        key.pos = ImVec2(black_key_x, start_pos.y);
        key.size = ImVec2(black_key_width, black_key_height);
        keys.push_back(key);
    }
}

// Draw the piano keys with note labels
void DrawPianoKeys()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 label_offset_white_keys(0.0f, 10.0f);        // Offset to position the label 
    ImVec2 label_offset_black_keys(0.0f, -20.0f);       // Offset to position the label 

    // First draw white keys so black keys are on top
    for (auto& key : keys)
    {
        if (key.is_white)
        {
            ImU32 color = key.pressed ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 255, 255, 255);
            draw_list->AddRectFilled(key.pos, ImVec2(key.pos.x + key.size.x, key.pos.y + key.size.y), color);
            draw_list->AddRect(key.pos, ImVec2(key.pos.x + key.size.x, key.pos.y + key.size.y), IM_COL32(0, 0, 0, 255));

            // Add labels for white keys
            ImVec2 text_pos = ImVec2(key.pos.x + key.size.x * 0.5f, key.pos.y + key.size.y + label_offset_white_keys.y);
            const char* note_label = ""; // Default empty label
            switch (key.key)
            {
            case '1': note_label = "C3"; break;
            case '2': note_label = "E3"; break;
            case '3': note_label = "G3"; break;
            case '4': note_label = "C4"; break;
            case '5': note_label = "E4"; break;
            case '6': note_label = "G4"; break;
            case '7': note_label = "C5"; break;
            case '8': note_label = "E5"; break;
            case '9': note_label = "G5"; break;
            case '0': note_label = "C6"; break;
            }
            draw_list->AddText(ImVec2(text_pos.x - ImGui::CalcTextSize(note_label).x * 0.5f, text_pos.y), IM_COL32(255, 255, 255, 255), note_label);
        }
    }

    // Then draw black keys
    for (auto& key : keys)
    {
        if (!key.is_white)
        {
            ImU32 color = key.pressed ? IM_COL32(0, 255, 0, 255) : IM_COL32(0, 0, 0, 255);
            draw_list->AddRectFilled(key.pos, ImVec2(key.pos.x + key.size.x, key.pos.y + key.size.y), color);
            draw_list->AddRect(key.pos, ImVec2(key.pos.x + key.size.x, key.pos.y + key.size.y), IM_COL32(0, 0, 0, 255));

            // Add labels for black keys
            ImVec2 text_pos = ImVec2(key.pos.x + key.size.x * 0.5f, key.pos.y + key.size.y + label_offset_black_keys.y);
            const char* note_label = ""; // Default empty label
            switch (key.key)
            {
            case 'Q': note_label = "D3"; break;
            case 'W': note_label = "F3"; break;
            case 'E': note_label = "A3"; break;
            case 'R': note_label = "D4"; break;
            case 'T': note_label = "F4"; break;
            case 'Y': note_label = "A4"; break;
            case 'U': note_label = "D5"; break;
            case 'I': note_label = "F5"; break;
            case 'O': note_label = "A5"; break;
            case 'P': note_label = "D6"; break;
            }
            draw_list->AddText(ImVec2(text_pos.x - ImGui::CalcTextSize(note_label).x * 0.5f, text_pos.y), IM_COL32(255, 255, 255, 255), note_label);
        }
    }
}

void UpdatePianoKeys(float deltaTime)
{
    ImGuiIO& io = ImGui::GetIO();
    for (auto& key : keys)
    {
        bool keyPressed = io.KeysDown[(int)key.key];

        if (keyPressed && !key.pressed)
        {
            // Generate a new note starting at the top of the key
            Note new_note;
            new_note.pos = ImVec2(key.pos.x, key.pos.y - 10.0f);    // Start note at the top of the key
            new_note.size = ImVec2(key.size.x, 1.0f);               // Initial height of the note
            new_note.color = IM_COL32(255, 0, 0, 255);              // Color of the note
            new_note.locked = false;                                // Note is not locked initially
            active_notes.push_back(new_note);

            // Play the sound corresponding to this key
            soundEngine->play2D(keySounds[key.key], false);

            // Set the key's pressed state
            key.pressed = true;
        }
        else if (keyPressed && key.pressed)
        {
            // Elongate the note while the key is pressed
            for (auto& note : active_notes)
            {
                if (!note.locked && note.pos.x == key.pos.x)
                {
                    note.size.y += 100.0f * deltaTime;  // Increase the height of the note
                }
            }
        }
        else if (!keyPressed && key.pressed)
        {
            // Lock the note's size when the key is released
            for (auto& note : active_notes)
            {
                if (note.pos.x == key.pos.x)
                {
                    note.locked = true;
                }
            }

            // The key was just released
            key.pressed = false;
        }
    }

    // Update notes (move upwards)
    UpdateNotes(deltaTime);
}

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Add these two variables to manage the key mapping window
bool showKeyMappingWindow = false;

// Main code
int main(int, char**)
{
    // Initialize sound engine
    soundEngine = createIrrKlangDevice();
    if (!soundEngine)
        return 0;       // Error starting up the sound engine

    LoadKeySounds();            // Load key sounds
    InitializeKeyMappings();    // Initialize key mappings
    UpdateKeySounds();          // Load initial key sounds
    
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Syntezator", WS_OVERLAPPEDWINDOW, 25, 25, 1500, 750, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0, 0, 0, 1.00f);

    InitPianoKeys();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle lost D3D9 device
        if (g_DeviceLost)
        {
            HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST)
            {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET)
                ResetDevice();
            g_DeviceLost = false;
        }

        // Handle window resize
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //-------------------------------------------------------------------------------------------------------------------------------------

        // Show key mapping window
        if (showKeyMappingWindow)
        {
            ShowKeyMappingWindow(&showKeyMappingWindow);
        }

        // Update and draw piano keys and notes if key mapping is not active
        if (!isKeyMappingActive)
        {
            float deltaTime = ImGui::GetIO().DeltaTime;
            UpdatePianoKeys(deltaTime);
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y)); // Set the menu width to 200 pixels
            if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
            {
                if (ImGui::Button("Save"))
                {
                    // Handle "Save" button click
                }

                if (ImGui::Button("Save as"))
                {
                    // Handle "Save as" button click
                }

                if (ImGui::Button("Load"))
                {
                    // Handle "Load" button click
                }

                if (ImGui::BeginMenu("Settings"))
                {
                    if (ImGui::MenuItem("Change key mappings"))
                    {
                        showKeyMappingWindow = true;
                    }

                    ImGui::EndMenu();
                }
            }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(200, 0));                // Move the piano window to start right after the menu
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);   // Adjust the size accordingly
            if (ImGui::Begin("Piano Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
            {
                DrawPianoKeys();
                DrawNotes();
            }
            ImGui::End();
        }

        //-------------------------------------------------------------------------------------------------------------------------------------

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && !g_DeviceLost)
            g_DeviceLost = true;
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }

    if (g_pD3D)
    {
        g_pD3D->Release();
        g_pD3D = nullptr;
    }

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
