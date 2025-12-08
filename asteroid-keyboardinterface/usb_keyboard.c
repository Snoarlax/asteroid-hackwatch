// usb_keyboard.c - USB HID Keyboard Interface with Delay Support
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

// Modifier key flags
#define MOD_LCTRL   0x01
#define MOD_LSHIFT  0x02
#define MOD_LALT    0x04
#define MOD_LGUI    0x08
#define MOD_RCTRL   0x10
#define MOD_RSHIFT  0x20
#define MOD_RALT    0x40
#define MOD_RGUI    0x80

// Special key names and their keycodes
typedef struct {
    const char *name;
    unsigned char code;
} KeyMapping;

// USB HID Keycodes
static const KeyMapping key_map[] = {
    {"a", 0x04}, {"b", 0x05}, {"c", 0x06}, {"d", 0x07},
    {"e", 0x08}, {"f", 0x09}, {"g", 0x0a}, {"h", 0x0b},
    {"i", 0x0c}, {"j", 0x0d}, {"k", 0x0e}, {"l", 0x0f},
    {"m", 0x10}, {"n", 0x11}, {"o", 0x12}, {"p", 0x13},
    {"q", 0x14}, {"r", 0x15}, {"s", 0x16}, {"t", 0x17},
    {"u", 0x18}, {"v", 0x19}, {"w", 0x1a}, {"x", 0x1b},
    {"y", 0x1c}, {"z", 0x1d},
    {"1", 0x1e}, {"2", 0x1f}, {"3", 0x20}, {"4", 0x21},
    {"5", 0x22}, {"6", 0x23}, {"7", 0x24}, {"8", 0x25},
    {"9", 0x26}, {"0", 0x27},
    {"enter", 0x28}, {"esc", 0x29}, {"escape", 0x29},
    {"backspace", 0x2a}, {"tab", 0x2b}, {"space", 0x2c},
    {"-", 0x2d}, {"=", 0x2e}, {"[", 0x2f}, {"]", 0x30},
    {"\\", 0x31}, {";", 0x33}, {"'", 0x34}, {"`", 0x35},
    {",", 0x36}, {".", 0x37}, {"/", 0x38},
    {"capslock", 0x39},
    {"f1", 0x3a}, {"f2", 0x3b}, {"f3", 0x3c}, {"f4", 0x3d},
    {"f5", 0x3e}, {"f6", 0x3f}, {"f7", 0x40}, {"f8", 0x41},
    {"f9", 0x42}, {"f10", 0x43}, {"f11", 0x44}, {"f12", 0x45},
    {"printscreen", 0x46}, {"scrolllock", 0x47}, {"pause", 0x48},
    {"insert", 0x49}, {"home", 0x4a}, {"pageup", 0x4b},
    {"delete", 0x4c}, {"end", 0x4d}, {"pagedown", 0x4e},
    {"right", 0x4f}, {"left", 0x50}, {"down", 0x51}, {"up", 0x52},
    {"leftmeta", 0xe3},{"rightmeta", 0xe7},
    {NULL, 0}
};

// Shifted character mappings
static const KeyMapping shift_map[] = {
    {"A", 0x04}, {"B", 0x05}, {"C", 0x06}, {"D", 0x07},
    {"E", 0x08}, {"F", 0x09}, {"G", 0x0a}, {"H", 0x0b},
    {"I", 0x0c}, {"J", 0x0d}, {"K", 0x0e}, {"L", 0x0f},
    {"M", 0x10}, {"N", 0x11}, {"O", 0x12}, {"P", 0x13},
    {"Q", 0x14}, {"R", 0x15}, {"S", 0x16}, {"T", 0x17},
    {"U", 0x18}, {"V", 0x19}, {"W", 0x1a}, {"X", 0x1b},
    {"Y", 0x1c}, {"Z", 0x1d},
    {"!", 0x1e}, {"\"", 0x1f}, {"£", 0x20}, {"$", 0x21},
    {"%", 0x22}, {"^", 0x23}, {"&", 0x24}, {"*", 0x25},
    {"(", 0x26}, {")", 0x27},
    {"_", 0x2d}, {"+", 0x2e}, {"{", 0x2f}, {"}", 0x30},
    {"|", 0x31}, {":", 0x33}, {"@", 0x34}, {"~", 0x35},
    {"<", 0x36}, {">", 0x37}, {"?", 0x38},
    {NULL, 0}
};

int hidg_fd = -1;

void send_report(unsigned char modifier, unsigned char key) {
    unsigned char report[8] = {0};

    // Handle 0xe0-0xe7 keycodes (these are actually modifiers in USB HID)
    if (key >= 0xe0 && key <= 0xe7) {
        // Convert keycode to modifier bit
        // 0xe0=LCTRL, 0xe1=LSHIFT, 0xe2=LALT, 0xe3=LGUI, 
        // 0xe4=RCTRL, 0xe5=RSHIFT, 0xe6=RALT, 0xe7=RGUI
        report[0] = 1 << (key - 0xe0);
        report[2] = 0;
    } else {
        report[0] = modifier;
        report[2] = key;
    }

    if (write(hidg_fd, report, 8) < 0) {
        perror("Failed to write key press");
    }
    usleep(10000); // 10ms delay
    
    // Release
    memset(report, 0, 8);
    if (write(hidg_fd, report, 8) < 0) {
        perror("Failed to write key release");
    }
    usleep(10000); // 10ms delay
}

unsigned char find_keycode(const char *key, int *needs_shift) {
    *needs_shift = 0;
    
    // Check regular keys
    for (int i = 0; key_map[i].name != NULL; i++) {
        if (strcmp(key, key_map[i].name) == 0) {
            return key_map[i].code;
        }
    }
    
    // Check shifted keys
    for (int i = 0; shift_map[i].name != NULL; i++) {
        if (strcmp(key, shift_map[i].name) == 0) {
            *needs_shift = 1;
            return shift_map[i].code;
        }
    }
    
    return 0;
}

void type_char(char c) {
    char str[2] = {c, '\0'};
    int needs_shift;
    unsigned char keycode;
    if (c == ' ') {
        keycode = 0x2c; // Space keycode
        send_report(0, keycode);
        return;
    }
    
    keycode = find_keycode(str, &needs_shift);
    if (keycode) {
        unsigned char modifier = needs_shift ? MOD_LSHIFT : 0;
        send_report(modifier, keycode);
    }
}

void type_string(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        type_char(text[i]);
    }
}

void send_key_combo(const char *combo) {
    char buffer[256];
    strncpy(buffer, combo, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    // Parse modifiers and key
    unsigned char modifier = 0;
    char *token = strtok(buffer, "+");
    char *last_token = NULL;
    
    while (token != NULL) {
        // Trim whitespace
        /*while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') end--;
        *(end + 1) = '\0';
        */
        
        // Convert to lowercase for comparison
        char lower[256];
        for (int i = 0; token[i]; i++) {
            lower[i] = tolower(token[i]);
        }
        lower[strlen(token)] = '\0';
        
        if (strcmp(lower, "ctrl") == 0 || strcmp(lower, "control") == 0) {
            modifier |= MOD_LCTRL;
        } else if (strcmp(lower, "shift") == 0) {
            modifier |= MOD_LSHIFT;
        } else if (strcmp(lower, "alt") == 0) {
            modifier |= MOD_LALT;
        } else if (strcmp(lower, "gui") == 0 || strcmp(lower, "win") == 0 || strcmp(lower, "cmd") == 0) {
            modifier |= MOD_LGUI;
        } else {
            last_token = token;
        }
        
        token = strtok(NULL, "+");
    }
    
    // Send the combination
    if (last_token) {
        int needs_shift;
        unsigned char keycode = find_keycode(last_token, &needs_shift);
        if (needs_shift) {
            modifier |= MOD_LSHIFT;
        }
        
        if (keycode) {
            send_report(modifier, keycode);
        } else {
            fprintf(stderr, "Unknown key: %s\n", last_token);
        }
    }
}

int parse_delay(const char *line, double *delay_seconds) {
    char cmd[32];
    double value;
    char unit[32] = "s"; // default to seconds
    
    // Try to parse: delay <number> [unit]
    // Supported formats:
    // delay 2
    // delay 2s
    // delay 500ms
    // delay 2.5
    // sleep 2
    
    int matched = sscanf(line, "%31s %lf%31s", cmd, &value, unit);
    
    if (matched < 2) {
        return 0; // Not a delay command
    }
    
    // Check if it's a delay/sleep/wait command
    char lower_cmd[32];
    for (int i = 0; cmd[i]; i++) {
        lower_cmd[i] = tolower(cmd[i]);
    }
    lower_cmd[strlen(cmd)] = '\0';
    
    if (strcmp(lower_cmd, "delay") != 0 && 
        strcmp(lower_cmd, "sleep") != 0 && 
        strcmp(lower_cmd, "wait") != 0) {
        return 0; // Not a delay command
    }
    
    // Convert to seconds based on unit
    if (matched == 2 || strcmp(unit, "s") == 0 || strcmp(unit, "sec") == 0 || strcmp(unit, "seconds") == 0) {
        *delay_seconds = value;
    } else if (strcmp(unit, "ms") == 0 || strcmp(unit, "milliseconds") == 0) {
        *delay_seconds = value / 1000.0;
    } else if (strcmp(unit, "m") == 0 || strcmp(unit, "min") == 0 || strcmp(unit, "minutes") == 0) {
        *delay_seconds = value * 60.0;
    } else {
        fprintf(stderr, "Unknown time unit: %s (assuming seconds)\n", unit);
        *delay_seconds = value;
    }
    
    return 1; // Successfully parsed delay
}

void process_line(char *line) {
    // Remove newline
    line[strcspn(line, "\r\n")] = '\0';
    
    // Skip empty lines
    if (strlen(line) == 0) {
        return;
    }
    
    // Skip comments (lines starting with #)
    if (line[0] == '#') {
        return;
    }
    
    // Check for delay command
    double delay_seconds;
    if (parse_delay(line, &delay_seconds)) {
        printf("Waiting %.3f seconds...\n", delay_seconds);
        usleep((useconds_t)(delay_seconds * 1000000));
        return;
    }
    
    // Check if it's a key combination (contains +)
    if (strchr(line, '+') != NULL) {
        printf("Sending key combo: %s\n", line);
        send_key_combo(line);
    } else if (strcasecmp(line, "enter") == 0 || 
               strcasecmp(line, "tab") == 0 ||
               strcasecmp(line, "escape") == 0 ||
               strcasecmp(line, "esc") == 0 ||
               strcasecmp(line, "backspace") == 0 ||
               strcasecmp(line, "space") == 0 ||
               strcasecmp(line, "delete") == 0 ||
               strcasecmp(line, "up") == 0 ||
               strcasecmp(line, "down") == 0 ||
               strcasecmp(line, "left") == 0 ||
               strcasecmp(line, "right") == 0 ||
               strcasecmp(line, "home") == 0 ||
               strcasecmp(line, "end") == 0 ||
               strcasecmp(line, "pageup") == 0 ||
               strcasecmp(line, "pagedown") == 0 ||
               strcasecmp(line, "leftmeta") == 0 ||
               strcasecmp(line, "rightmeta") == 0 ||
               strncasecmp(line, "f", 1) == 0) {
        // Special key
        printf("Sending special key: %s\n", line);
        int needs_shift;
        unsigned char keycode = find_keycode(line, &needs_shift);
        if (keycode) {
            send_report(0, keycode);
        } else {
            fprintf(stderr, "Unknown special key: %s\n", line);
        }
    } else {
        // Regular text to type
        printf("Typing: %s\n", line);
        type_string(line);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <keystroke_file>\n", argv[0]);
        fprintf(stderr, "\nFile format:\n");
        fprintf(stderr, "  - Plain text will be typed character by character\n");
        fprintf(stderr, "  - Special keys: enter, tab, escape, backspace, etc.\n");
        fprintf(stderr, "  - Key combinations: ctrl+alt+t, shift+a, etc.\n");
        fprintf(stderr, "  - Delays: delay <time> [unit]\n");
        fprintf(stderr, "    - delay 2        (2 seconds)\n");
        fprintf(stderr, "    - delay 500ms    (500 milliseconds)\n");
        fprintf(stderr, "    - delay 1.5s     (1.5 seconds)\n");
        fprintf(stderr, "    - sleep 3        (3 seconds)\n");
        fprintf(stderr, "    - wait 2m        (2 minutes)\n");
        fprintf(stderr, "  - Comments: lines starting with #\n");
        fprintf(stderr, "\nExample file:\n");
        fprintf(stderr, "  # Open jerminal\n");
        fprintf(stderr, "  ctrl+alt+t\n");
        fprintf(stderr, "  delay 1\n");
        fprintf(stderr, "  whoami\n");
        fprintf(stderr, "  enter\n");
        return 1;
    }
    
    // Open HID device
    hidg_fd = open("/dev/hidg0", O_RDWR);
    if (hidg_fd < 0) {
        perror("Failed to open /dev/hidg0");
        return 1;
    }
    
    // Open input file
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Failed to open input file");
        close(hidg_fd);
        return 1;
    }
    
    // Process each line
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        process_line(line);
    }
    
    fclose(fp);
    close(hidg_fd);
    
    printf("Done!\n");
    return 0;
}
