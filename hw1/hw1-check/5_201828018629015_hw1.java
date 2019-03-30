import java.io.*;
import java.util.*;
import java.net.URI;
import java.net.URISyntaxException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IOUtils;

import java.io.IOException;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.HTableDescriptor;
import org.apache.hadoop.hbase.MasterNotRunningException;
import org.apache.hadoop.hbase.TableName;
import org.apache.hadoop.hbase.ZooKeeperConnectionException;
import org.apache.hadoop.hbase.client.HBaseAdmin;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.log4j.*;

public class Hw1Grp5 {
    public static boolean checkSame(ArrayList<String> s1, ArrayList<String> s2, ArrayList<Integer> nums){
        for(int i=0;i<nums.size();i++){
            if(!s1.get(nums.get(i)).equals(s2.get(nums.get(i)))) {
                return false;
            }
        }
        return true;
    }
    public static void main(String[] args) throws IOException, URISyntaxException{
        /*
        Get parameters from command line
        Give the vairables with corresponding values
         */
        String fileName = args[0].substring(2,args[0].length());
        int optCol = Integer.parseInt(args[1].substring(8,9));
        String opt = args[1].substring(10,12);
        float optValue = Float.parseFloat(args[1].substring(13,args[1].length()));
        ArrayList<Integer> disCol = new ArrayList<Integer>();
        for(int i=0; i<args[2].length(); i++){
            if(args[2].charAt(i)=='R'){
                disCol.add(Integer.parseInt(String.valueOf(args[2].charAt(i+1))));
            }
        }

        /*
        Use a list of list to store all the table data,
        one inner list maps a row in the table
         */

        List<ArrayList<String>> text = new ArrayList<ArrayList<String>>();
        /*
        Read data operation
         */
        Configuration conf = new Configuration();
        FileSystem fs = FileSystem.get(URI.create(fileName), conf);
        Path path = new Path(fileName);
        FSDataInputStream in_stream = fs.open(path);
        BufferedReader in = new BufferedReader(new InputStreamReader(in_stream));
        String str;
        while ((str=in.readLine())!=null){
            String[] aa = str.split("\\|");
            ArrayList<String> temp = new ArrayList<String>();
            for(int i=0;i<aa.length;i++) {
                temp.add(aa[i]);
            }
            text.add(temp);
        }
        in.close();
        fs.close();
    
        /*
        Select which operator to use
         */
        if(opt.equals("gt")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) <= optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("ge")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) < optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("eq")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) != optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("ne")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) == optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("le")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) > optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("lt")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) >= optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }


        /*
        Override comparison function
         */
        Collections.sort(text, new Comparator<ArrayList<String>>() {
            @Override
            public int compare(ArrayList<String> o1, ArrayList<String> o2) {
                for(int i=0;i<disCol.size();i++) {
                    int result = o1.get(disCol.get(i)).compareTo(o2.get(disCol.get(i)));
                    if (result != 0) {
                        return result;
                    }
                }
                return 0;
            }
        });
        /*
        Remove same entries
         */
        for(int i=0;i<text.size()-1;i++){
            if(checkSame(text.get(i),text.get(i+1),disCol)){
                text.remove(text.get(i+1));
                i--;
            }
        }
        
        /*
        Write data into HBase
        */
        Logger.getRootLogger().setLevel(Level.WARN);
        // create table descriptor
        String tableName = "Result";
        HTableDescriptor htd = new HTableDescriptor(TableName.valueOf(tableName));

        // create column descriptor
        HColumnDescriptor cf = new HColumnDescriptor("res");
        htd.addFamily(cf);

        // configure HBase
        Configuration configuration = HBaseConfiguration.create();
        HBaseAdmin hAdmin = new HBaseAdmin(configuration);

        if (hAdmin.tableExists(tableName)) {
            System.out.println("Table already exists");
        }
        else {
            hAdmin.createTable(htd);
            System.out.println("table "+tableName+ " created successfully");
        }
        hAdmin.close();

        HTable table = new HTable(configuration,tableName);
        for(int i=0;i<text.size();i++){
            Put put = new Put((""+i).getBytes());
            for(int j=0;j<disCol.size();j++){
                put.add("res".getBytes(),("R"+disCol.get(j)).getBytes(),text.get(i).get(disCol.get(j)).getBytes());
            }
            table.put(put);
        }
        table.close();
        System.out.println("put successfully");
    }
}
