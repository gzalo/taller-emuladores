import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;

public class Taller implements KeyListener {
    private final JFrame frame;
    private final MyCanvas canvas;
    private final BufferedImage image;
    private boolean keyDown = false;
    private final int scale = 4;
    private final int width = 64;
    private final int height = 32;
    public Taller() {
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        frame = new JFrame("");
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.setSize(width*scale, height*scale);
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);
        frame.addKeyListener(this);
        int titleBarHeight = frame.getInsets().top;
        frame.setSize(width*scale, height*scale+titleBarHeight);

        canvas = new MyCanvas();
        canvas.setSize(width*scale, height*scale);
        frame.add(canvas);

        while (true) {
            canvas.repaint();

            try {
                Thread.sleep(16);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    private class MyCanvas extends JPanel {
        @Override

        public void paintComponent(Graphics g) {
            super.paintComponent(g);

            if (keyDown) {
                image.setRGB(10, 10, 0xFFFFFF);
            } else {
                image.setRGB(10, 10, 0x00);
            }

            g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
        }
    }

    public static void main(String[] args) {
        new Taller();
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    @Override
    public void keyPressed(KeyEvent e) {
        if(e.getKeyCode() == 49){
            keyDown = true;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        if(e.getKeyCode() == 49){
            keyDown = false;
        }
    }
}