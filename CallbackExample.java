public class CallbackExample {
    static {
        System.loadLibrary("nativeLib");
        System.out.println("load complete");
    }

    // Java回调数据类
    public static class CallbackData {
        private String name;
        private int intValue;
        private double doubleValue;
        private long longValue;

        public CallbackData(){
            
        }

        public CallbackData(String name, int intValue, double doubleValue, long longValue) {
            this.name = name;
            this.intValue = intValue;
            this.doubleValue = doubleValue;
            this.longValue = longValue;
        }

        // Getters and setters
        public String getName() {
            return name;
        }

        public int getIntValue() {
            return intValue;
        }

        public double getDoubleValue() {
            return doubleValue;
        }

        public long getLongValue() {
            return longValue;
        }

        // Setters
        public void setName(String name) {
            this.name = name;
        }

        public void setIntValue(int intValue) {
            this.intValue = intValue;
        }

        public void setDoubleValue(double doubleValue) {
            this.doubleValue = doubleValue;
        }

        public void setLongValue(long longValue) {
            this.longValue = longValue;
        }
    }

    // 定义回调函数接口
    public interface Callback {
        void onCallback(CallbackData data);
    }

    // 注册回调函数
    public native void registerCallback(int id, Callback callback);

    // 调用C++业务方法
    public native void startBusiness(int id);

    public static void main(String[] args) {
        System.out.println(" enter main");

        System.out.println(" start call new CallbackExample() ");

        CallbackExample example = new CallbackExample();
        System.out.println(" end call new CallbackExample() ");

        System.out.println(" start call registerCallback ");

        example.registerCallback(1, data -> {
            System.out.println("Name: " + data.getName());
            System.out.println("Int Value: " + data.getIntValue());
            System.out.println("Double Value: " + data.getDoubleValue());
            System.out.println("Long Value: " + data.getLongValue());
        });
        System.out.println(" end call registerCallback ");


        System.out.println(" start call startBusiness ");

        example.startBusiness(1);
        System.out.println(" end call startBusiness ");

    }
}
