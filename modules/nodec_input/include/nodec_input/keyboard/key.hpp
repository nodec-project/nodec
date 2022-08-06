#ifndef NODEC_INPUT__KEYBOARD__KEY_HPP_
#define NODEC_INPUT__KEYBOARD__KEY_HPP_


namespace nodec_input {
namespace keyboard {

// Each code value is based on the following specifications.
//  Windows:
//      * https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//  Qt:
//      * https://doc.qt.io/qt-6/qt.html#Key-enum
//
//  Unity:
//      * https://docs.unity3d.com/2021.2/Documentation/ScriptReference/KeyCode.html


enum class Key {
    None = 0x00,

    Return = 0x0D,
    Escape = 0x1B,

    Space = 0x20, // ' '

    LeftArrow = 0x25,
    UpArrow = 0x26,
    RightArrow = 0x27,
    DownArrow = 0x28,

    Alpha0 = 0x30, // The '0' key on the top of the alphanumeric keyboard.
    Alpha1 = 0x31,
    Alpha2 = 0x32,
    Alpha3 = 0x33,
    Alpha4 = 0x34,
    Alpha5 = 0x35,
    Alpha6 = 0x36,
    Alpha7 = 0x37,
    Alpha8 = 0x38,
    Alpha9 = 0x39,

    A = 0x41, // 'A'
    B = 0x42, // 'B'
    C = 0x43, // 'C'
    D = 0x44, // 'D'
    E = 0x45, // 'E'
    F = 0x46, // 'F'
    G = 0x47, // 'G'
    H = 0x48, // 'H'
    I = 0x49, // 'I'
    J = 0x4A, // 'J'
    K = 0x4B, // 'K'
    L = 0x4C, // 'L'
    M = 0x4D, // 'M'
    N = 0x4E, // 'N'
    O = 0x4F, // 'O'
    P = 0x50, // 'P'
    Q = 0x51, // 'Q'
    R = 0x52, // 'R'
    S = 0x53, // 'S'
    T = 0x54, // 'T'
    U = 0x55, // 'U'
    V = 0x56, // 'V'
    W = 0x57, // 'W'
    X = 0x58, // 'X'
    Y = 0x59, // 'Y'
    Z = 0x5A, // 'Z'

    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,
    F13 = 0x7C,
    F14 = 0x7D,
    F15 = 0x7E,
    F16 = 0x7F,
    F17 = 0x80,
    F18 = 0x81,
    F19 = 0x82,
    F20 = 0x83,
    F21 = 0x84,
    F22 = 0x85,
    F23 = 0x86,
    F24 = 0x87,

};


inline std::ostream& operator<<(std::ostream& stream, const Key& key) {
    switch (key) {
    case Key::None: return stream << "None";

    case Key::Return: return stream << "Return";
    case Key::Escape: return stream << "Escape";

    case Key::Space: return stream << "Space";

    case Key::LeftArrow:  return stream << "LeftArrow";
    case Key::UpArrow:    return stream << "UpArrow";
    case Key::RightArrow: return stream << "RightArrow";
    case Key::DownArrow:  return stream << "DownArrow";

    case Key::Alpha0: return stream << "Alpha0";
    case Key::Alpha1: return stream << "Alpha1";
    case Key::Alpha2: return stream << "Alpha2";
    case Key::Alpha3: return stream << "Alpha3";
    case Key::Alpha4: return stream << "Alpha4";
    case Key::Alpha5: return stream << "Alpha5";
    case Key::Alpha6: return stream << "Alpha6";
    case Key::Alpha7: return stream << "Alpha7";
    case Key::Alpha8: return stream << "Alpha8";
    case Key::Alpha9: return stream << "Alpha9";

    case Key::A: return stream << "A";
    case Key::B: return stream << "B";
    case Key::C: return stream << "C";
    case Key::D: return stream << "D";
    case Key::E: return stream << "E";
    case Key::F: return stream << "F";
    case Key::G: return stream << "G";
    case Key::H: return stream << "H";
    case Key::I: return stream << "I";
    case Key::J: return stream << "J";
    case Key::K: return stream << "K";
    case Key::L: return stream << "L";
    case Key::M: return stream << "M";
    case Key::N: return stream << "N";
    case Key::O: return stream << "O";
    case Key::P: return stream << "P";
    case Key::Q: return stream << "Q";
    case Key::R: return stream << "R";
    case Key::S: return stream << "S";
    case Key::T: return stream << "T";
    case Key::U: return stream << "U";
    case Key::V: return stream << "V";
    case Key::W: return stream << "W";
    case Key::X: return stream << "X";
    case Key::Y: return stream << "Y";
    case Key::Z: return stream << "Z";

    case Key::F1: return stream << "F1";
    case Key::F2: return stream << "F2";
    case Key::F3: return stream << "F3";
    case Key::F4: return stream << "F4";
    case Key::F5: return stream << "F5";
    case Key::F6: return stream << "F6";
    case Key::F7: return stream << "F7";
    case Key::F8: return stream << "F8";
    case Key::F9: return stream << "F9";
    case Key::F10: return stream << "F10";
    case Key::F11: return stream << "F11";
    case Key::F12: return stream << "F12";
    case Key::F13: return stream << "F13";
    case Key::F14: return stream << "F14";
    case Key::F15: return stream << "F15";
    case Key::F16: return stream << "F16";
    case Key::F17: return stream << "F17";
    case Key::F18: return stream << "F18";
    case Key::F19: return stream << "F19";
    case Key::F20: return stream << "F20";
    case Key::F21: return stream << "F21";
    case Key::F22: return stream << "F22";
    case Key::F23: return stream << "F23";
    case Key::F24: return stream << "F24";


    default: return stream << "Unknown";
    }
}

}
}


#endif