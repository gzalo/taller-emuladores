extern crate sdl2;

use std::fs::File;
use std::io::Read;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::rect::Rect;
use std::time::Duration;

const SCALE: u32 = 4;
const WIDTH: u32 = 64;
const HEIGHT: u32 = 32;
const SCREEN_SIZE: usize = (WIDTH * HEIGHT) as usize;

fn set_pixel(screen: &mut [u8], x: u8, y: u8, value: u32) {
    let index = ((y as u32 * WIDTH + x as u32)*4) as usize;
    screen[index] = (value & 0xFF) as u8;
}

fn get_pixel(screen: &[u8], x: u8, y: u8) -> u32 {
    let index = ((y as u32 * WIDTH + x as u32)*4) as usize;
    return screen[index] as u32;
}

fn main() {
    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();
    let window = video_subsystem
        .window("", WIDTH * SCALE, HEIGHT * SCALE)
        .position_centered()
        .build()
        .unwrap();
    let mut canvas = window.into_canvas().build().unwrap();
    let texture_creator = canvas.texture_creator();
    let mut texture = texture_creator
        .create_texture_streaming(
            sdl2::pixels::PixelFormatEnum::ARGB8888,
            WIDTH,
            HEIGHT,
        )
        .unwrap();

    let mut screen: [u8; SCREEN_SIZE*4] = [0; SCREEN_SIZE*4];

    let mut memory: [u8; 0x1000] = [0; 0x1000];
    let mut v: [u8; 16] = [0; 16];
    let mut pc: u16 = 0x200;
    let mut idx: u16 = 0;

    let mut f = File::open("1-chip8-logo.ch8").unwrap();
    let mut buffer: Vec<u8> = Vec::new();
    f.read_to_end(&mut buffer).unwrap();
    for i in 0..buffer.len() {
        if i >= 0xE00 {
            break;
        }
        memory[i+0x200] = buffer[i];
    }

    let mut event_pump = sdl_context.event_pump().unwrap();
    let mut quit = false;
    while !quit {
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. } => quit = true,
                Event::KeyDown {
                    keycode: Some(Keycode::Num1),
                    ..
                } => set_pixel(&mut screen, 10, 10, 0xFFFFFFFF),
                Event::KeyUp {
                    keycode: Some(Keycode::Num1),
                    ..
                } => set_pixel(&mut screen, 10, 10, 0x00000000),
                _ => {}
            }
        }

        let opcode: u16 = ((memory[pc as usize] as u16) << 8) | memory[(pc+1) as usize] as u16;
        pc += 2;

        let nibble1: u8 = (opcode >> 12) as u8;
        let nibble2: u8 = ((opcode >> 8) & 0xF) as u8;
        let nibble3: u8 = ((opcode >> 4) & 0xF) as u8;
        let nibble4: u8 = (opcode & 0xF) as u8;
        let address: u16 = opcode & 0xFFF;
        let byte2: u8 = (opcode & 0xFF) as u8;

        if nibble1 == 0x0 {
            if address == 0x0E0 {
                for y in 0..HEIGHT as u8 {
                    for x in 0..WIDTH as u8 {
                        set_pixel(&mut screen, x, y, 0x00000000);
                    }
                }
            }
        } else if nibble1 == 0x6 {
            v[nibble2 as usize] = byte2;
        } else if nibble1 == 0xA {
            idx = address;
        } else if nibble1 == 0xD {
            v[0x0F] = 0;
            for y in 0..nibble4 {
                let mut actual: u8 = memory[(idx+y as u16) as usize];
                for x in 0..8 as u8 {
                    let xnew = x+v[nibble2 as usize];
                    let ynew = y+v[nibble3 as usize];
                    if (actual & 0x80) != 0 && xnew < 64 && ynew <32 {
                        if get_pixel(&mut screen, xnew, ynew) != 0 {
                            set_pixel(&mut screen, xnew, ynew, 0);
                            v[0x0F] = 1;
                        } else {
                            set_pixel(&mut screen, xnew, ynew, 0xFFFFFF);
                        }
                    }
                    actual <<= 1;
                }
            }
        } else if nibble1 == 0x1 {
            pc = address;
        }

        texture
            .update(None,  &screen, (WIDTH * 4) as usize)
            .unwrap();

        canvas.clear();
        canvas.copy(&texture, None, Some(Rect::new(0, 0, WIDTH * SCALE, HEIGHT * SCALE)))
            .unwrap();
        canvas.present();

        std::thread::sleep(Duration::from_millis(16));
    }
}