extern crate sdl2;

use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use std::time::Duration;

const SCALE: u32 = 4;
const WIDTH: u32 = 64;
const HEIGHT: u32 = 32;
const SCREEN_SIZE: usize = (WIDTH * HEIGHT) as usize;

fn set_pixel(screen: &mut [u32], x: u8, y: u8, value: u32) {
    let index = (y as u32 * WIDTH + x as u32) as usize;
    screen[index] = value;
}

fn get_pixel(screen: &[u32], x: u8, y: u8) -> u32 {
    let index = (y as u32 * WIDTH + x as u32) as usize;
    screen[index]
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

    let mut screen: [u32; SCREEN_SIZE] = [0; SCREEN_SIZE];

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

        texture
            .update(None, &screen, (WIDTH as usize * std::mem::size_of::<u32>()) as usize)
            .unwrap();

        canvas.clear();
        canvas.copy(&texture, None, Some(Rect::new(0, 0, WIDTH * SCALE, HEIGHT * SCALE)))
            .unwrap();
        canvas.present();

        std::thread::sleep(Duration::from_millis(16));
    }
}