import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.io.*;
import java.util.*;
public class Main{
    public static void main(String[] args) {
        int cacheSize=16;
        int associativity=8;
        int blockSize=64;
        String traceFilePath ="trace5.txt";
        Cachee cache =new Cachee(cacheSize, associativity, blockSize,traceFilePath);
        try {
            cache.createCache(traceFilePath);
        } catch (IOException e) {
            System.out.println("Error reading the trace file: " + e.getMessage());
        }
        cache.print();
    }
}
class Cachee{
        private int cacheSize;
        private int associativity;
        private int blockSize;
        private int numsets;
        private int setbits;
        private int tagbits;
        public int cacheHit=0;
        public int cacheMiss=0;
        public int flag=0;
        private String traceFilepath;
        ArrayList<ArrayList<Integer>> cache2D = new ArrayList<>();
        ArrayList<ArrayList<Long>> timeLRU = new ArrayList<>();
        ArrayList<Integer> availability=new ArrayList<>();
        int[] SetHits;
        int[] SetMisses;
    public Cachee(int cacheSize, int associativity, int blockSize,String traceFilepath) {
        this.cacheSize = cacheSize;
        this.associativity = associativity;
        this.blockSize = blockSize;
        this.numsets = (cacheSize * 1024) / (associativity * blockSize);
        this.setbits = (int) (Math.log(numsets) / Math.log(2));
        this.traceFilepath=traceFilepath;
        this.tagbits=32-setbits-6;
        SetHits=new int[numsets];
        SetMisses=new int[numsets];
        for (int i = 0; i < numsets; i++) {
            cache2D.add(new ArrayList<>());
            timeLRU.add(new ArrayList<>());
            for(int j=0;j<associativity;j++){
                timeLRU.get(i).add(Long.MAX_VALUE);
                cache2D.get(i).add(-1);
            }
            availability.add(0);
            SetMisses[i]=0;
            SetHits[i]=0;
        }
    }
    public String toBinary(String input){
                String hexAddress = input;
                hexAddress=hexAddress.replace("0x","");
                long longAddress = Long.parseLong(hexAddress, 16);
                String binary=String.format("%32s",Long.toBinaryString(longAddress)).replace(" ","0"); 
                return binary;          
    }
    public void print(){
        for(int i=0;i<numsets;i++){
            System.out.println("Set"+ i+ " Hits: "+SetHits[i]);
            System.out.println("Set"+ i+ " Misses: "+SetMisses[i]);
        }
        System.out.println("CacheHits: "+cacheHit);
        System.out.println("CacheMisses: "+cacheMiss);
    }
    public void createCache(String traceFilePath) throws IOException {
        File file = new File(traceFilePath);
        Scanner sc = new Scanner(file);
        String line;
        long time=0;
        while (sc.hasNext()) {
            time++;
            line=sc.next();
           line=toBinary(line); 
           String tag="";
           String set="";
           for(int i=0;i<tagbits;i++){
            tag+=line.charAt(i);
           }
           for(int i=tagbits;i<tagbits+setbits;i++){
            set+=line.charAt(i);
           }
           boolean hit=false;
           int curSetIndex = Integer.parseInt(set, 2);
           int curTag = Integer.parseInt(tag,2);
           int index = 0;
            for (Integer tags: cache2D.get(curSetIndex)){
               if(tags==curTag){
                hit=true;
                SetHits[curSetIndex]+=1;
                timeLRU.get(curSetIndex).set(index,time); 
                break;
            }
            index++;
            }
            if(hit){
             cacheHit++;
            }
            else {
                SetMisses[curSetIndex]+=1;
                cacheMiss++;
                int available_pos;
                if (availability.get(curSetIndex) >= associativity) {
                    long minValue = Collections.min(timeLRU.get(curSetIndex));
                    int min_index=0;
                    int indexi=0;
                    long min_time=Long.MAX_VALUE;
                    for(long vals: timeLRU.get(curSetIndex) ){
                        if(min_time>vals){ min_index=indexi;
                        min_time=vals;
                        }
                        indexi++;
                    }
                    int minIndex = timeLRU.get(curSetIndex).indexOf(minValue);
                    available_pos = minIndex;
                    flag=1;
                    timeLRU.get(curSetIndex).set(min_index,time);
                    cache2D.get(curSetIndex).set(min_index, curTag);  
                }
                else{
                    available_pos = availability.get(curSetIndex);
                    availability.set(curSetIndex, available_pos + 1);
                    timeLRU.get(curSetIndex).set(available_pos,time);
                cache2D.get(curSetIndex).set(available_pos, curTag);  
                }
                }
        }
        sc.close();
    }  
}
