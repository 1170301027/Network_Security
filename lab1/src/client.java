import javax.xml.crypto.Data;
import java.io.IOException;
import java.net.*;

public class client {
    private final String sourceIP = "127.0.0.1";
    private final String targetIP;
    private final int sourcePort;
    private final int targetPort;

    public client(String targetIP, int sourcePort, int targetPort) {
        this.targetIP = targetIP;
        this.sourcePort = sourcePort;
        this.targetPort = targetPort;
    }

    /**
     * Scan the targetPort open or not
     * @return true if effiective.
     */
    public boolean getScanResult(){
        try {
            DatagramSocket socket = new DatagramSocket(sourcePort);
            SocketAddress socketAddress = new InetSocketAddress(targetIP,targetPort);
            socket.connect(socketAddress);
            Thread.sleep(2000);
            if(socket.isConnected()){
                socket.close();
                return true;
            } else {
                socket.close();
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static void main(String[] args) {
        UI ui = new UI();
        ui.graphics();

    }
}
