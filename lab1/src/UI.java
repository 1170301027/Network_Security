import com.sun.source.tree.NewArrayTree;
import javafx.scene.layout.Pane;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.nio.channels.NonWritableChannelException;
import java.util.ArrayList;
import java.util.List;

public class UI {
    private JFrame frame = new JFrame();
    private List<Thread> threadPool = new ArrayList<>();
    private TextArea records = new TextArea(); // one thread has a record
    private List<String> ipPool = new ArrayList<>(); // all ip in this pool
    private List<Integer> iptoport = new ArrayList<>();// scan completely when this is null
    private int MaxThread;
    private int portMax; //use to compare
    private int portMin;
    private int count = 0;
    private int sourcePort = 22222;

    /**
     * judge the thread is alive or not
     */
    private void recoveryThread(){
        List<Thread> integers = new ArrayList<>();
        for (Thread s : threadPool) {
            if (!s.isAlive()) {
                integers.add(s);
            }
        }
        threadPool.removeAll(integers);
        if (threadPool.size() < MaxThread) {//thread pool has free space
            System.out.println("new to allocate thread\n");
            synchronized (threadPool) {
                threadAllocate();
            }
        }
    }

    /**
     * Create new threads to fill threadPool.
     */
    private void threadAllocate(){

        for (int i=threadPool.size();i<MaxThread;i++){
            if (ipPool.isEmpty()){
                break;
            }
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    String currentIP = ipPool.get(0);
                    int currentPort = iptoport.get(0);
                    if(currentPort < portMax){
                        iptoport.set(0,currentPort+1);
                    } else {
                        iptoport.remove(0);
                        ipPool.remove(0);
                    }
                    if (sourcePort++<25000){
                        sourcePort = 22222;
                    }
                    client client = new client(currentIP,sourcePort,currentPort);
                    if (client.getScanResult()){
                        count++;
                        records.append("IP: "+currentIP+"; Port: "+currentPort+"; State: useful\n");
                    } else {
                        records.append("IP: "+currentIP+"; Port: "+currentPort+"; State: unuseful\n");
                    }
                }
            });
            thread.start();
            threadPool.add(thread);
        }
    }

    /**
     * Initialize ip's range use the user input
     * @param ipStart
     * @param ipEnd
     */
    private void getIpRange(String ipStart,String ipEnd){
        String[] start = ipStart.split("\\.");
        String[] end = ipEnd.split("\\.");
        //默认 192.168网段
        if (start[2].equals(end[2])){
            for (int i = Integer.parseInt(start[3]); i <= Integer.parseInt(end[3]); i++) {
                ipPool.add(start[0]+"."+start[1]+"."+start[2]+"."+i);
                System.out.println(ipPool.get(ipPool.size()-1));
            }
        } else {
            for (int i = Integer.parseInt(start[2]); i < Integer.parseInt(end[2]); i++) {
                if (i == Integer.parseInt(start[2])){
                    for (int i1 = Integer.parseInt(start[3]);i1<255;i++){
                        ipPool.add(start[0]+"."+start[1]+"."+i+"."+i1);
                    }
                    continue;
                }
                for (int i1 = 1;i1<255;i++){
                    ipPool.add(start[0]+"."+start[1]+"."+i+"."+i1);
                }
            }
        }
    }

    /**
     * Handler
     * @param ipStrat
     * @param ipEnd
     * @param portStart
     * @param portEnd
     * @param threads
     */
    private void handle(String ipStrat,String ipEnd,String portStart,String portEnd,String threads){
        getIpRange(ipStrat,ipEnd);
        this.MaxThread = Integer.parseInt(threads);
        this.portMax = Integer.parseInt(portEnd);
        this.portMin = Integer.parseInt(portStart);
        for (int i = 0 ; i < MaxThread;i++){ //initialize the ip's ports pool
            iptoport.add(this.portMin);
        }
    }

    /**
     * Design the UI
     */
    public void graphics(){
        this.frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // 关闭按钮的事件响应
        frame.setVisible(true);
        frame.setSize(800,400);
        frame.setLayout(new GridLayout(2,1));
        Panel panel1 = new Panel();
        Panel panel2 = new Panel();
        frame.add(panel1);
        frame.add(panel2);
        Label ipRange = new Label("IP range:");
        Label portRange = new Label("Port range:");
        Label threads = new Label("Thread number:");
        TextField ipStart = new TextField(15);
        TextField ipEnd = new TextField(15);
        TextField portStart = new TextField(15);
        TextField portEnd = new TextField(15);
        TextField threadNumber = new TextField(15);

        panel1.add(ipRange);
        panel1.add(ipStart);
        panel1.add(ipEnd);
        panel1.add(portRange);
        panel1.add(portStart);
        panel1.add(portEnd);
        panel1.add(threads);
        panel1.add(threadNumber);
        ipRange.setLocation(50,50);
        ipStart.setLocation(15,150);
        ipEnd.setLocation(75,150);
        portRange.setLocation(50,250);
        portStart.setLocation(15,350);
        portEnd.setLocation(75,350);
        threads.setLocation(50,450);
        threadNumber.setLocation(50,550);
        Label outputLabel = new Label("Output:");
        records.setEditable(false);
        panel2.add(outputLabel);
        panel2.add(records);

        Button scanButton = new Button("Scan");
        panel2.add(scanButton);
        scanButton.setLocation(590,380);
        scanButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                handle(ipStart.getText(),ipEnd.getText(),portStart.getText(),portEnd.getText(),threadNumber.getText());
                while (true){

                    try {
                        if(ipPool.isEmpty()){
                            Thread.sleep(2000);
                            records.append("\n\n Scan completely! the number of useful ports : "+count);
                            break;
                        }
                        recoveryThread();
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

    }
}
