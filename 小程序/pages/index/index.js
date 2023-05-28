// index.js
// 获取应用实例
const app = getApp()
// 引入SDK核心类
var QQMapWX = require('../lib/qqmap-wx-jssdk.js');
// 实例化API核心类
var qqmapsdk = new QQMapWX({
    key: '3CJBZ-D363U-VQMVQ-GBUJZ-OIEQZ-MOFJ4' // 必填 申请的开发者密钥
});
var zhuan_dingwei = require('../../utils/dingwei.js');
Page({
 
    /**
     * 页面的初始数据
     * 27.849442,112.924014
     */
    data: {
      
        latitude: 27.847498, //经纬度先预设一个，获取地址信息后再替换
        longitude: 112.929185,
        markers: [{
            title: "",
            state: "",
            id: 0,
            latitude: 27.849442,
            longitude: 112.924014,
            iconPath: "../img/map/mark_red.png", //图标路径
            width: 36,
            height: 36,
            callout: { //在markers上展示地址名称，根据需求是否需要
                content: "",
                color: "#000",
                display: "ALWAYS"
            }
        }, 
        // {
        //     title: "",
        //     state: "",
        //     id: 1,
        //     latitude: 31.8726,
        //     longitude: 116.65063,
        //     iconPath: "../img/map/mark_red.png", //图标路径
        //     width: 36,
        //     height: 36,
        //     callout: { //在markers上展示地址名称，根据需求是否需要
        //         content: "",
        //         color: "#000",
        //         display: "ALWAYS"
        //     }
        // }
      ],
    },
    //获取当前位置信息
    getUserLocation() {
        var that = this;
        wx.getSetting({
            success: (res) => {
                console.log(JSON.stringify(res))
                res.authSetting['scope.userLocation'] == undefined    //表示 初始化进入该页面
                res.authSetting['scope.userLocation'] == false    //表示 非初始化进入该页面,且未授权
                res.authSetting['scope.userLocation'] == true    //表示 地理位置授权
                if (res.authSetting['scope.userLocation'] != undefined && res.authSetting['scope.userLocation'] != true) {
                    wx.showModal({
                        title: '请求授权当前位置',
                        content: '需要获取您的地理位置，请确认授权',
                        success: function (res) {
                            if (res.cancel) {
                                wx.showToast({
                                    title: '拒绝授权',
                                    icon: 'none',
                                    duration: 1000
                                });
                            } else if (res.confirm) {
                                wx.openSetting({
                                    success: function (dataAu) {
                                        if (dataAu.authSetting["scope.userLocation"] == true) {
                                            wx.showToast({
                                                title: '授权成功',
                                                icon: 'success',
                                                duration: 1000
                                            });
                                            //再次授权，调用wx.getLocation的API
                                            wx.getLocation({
                                                type: 'wgs84',
                                                success(res) {
                                                    console.log(res);
                                                    that.setData({
                                                        latitude: res.latitude,
                                                        longitude: res.longitude
                                                    });
                                                }
                                            });
                                        } else {
                                            wx.showToast({
                                                title: '授权失败',
                                                icon: 'none',
                                                duration: 1000
                                            });
                                        }
                                    }
                                })
                            }
                        }
                    })
                } else if (res.authSetting['scope.userLocation'] == undefined) {
                    //调用wx.getLocation的API
                    wx.getLocation({
                        type: 'wgs84',
                        success(res) {
                            console.log("undefined", res);
                            that.setData({
                                latitude: res.latitude,
                                longitude: res.longitude
                            });
                        }
                    });
                } else {
                    //调用wx.getLocation的API
                    wx.getLocation({
                        type: 'wgs84',
                        success(res) {
                            console.log(res);
                            that.setData({
                                latitude: res.latitude,
                                longitude: res.longitude
                            });
                        }
                    });
                }
            }
        })
    },
    //点击标注
    markertap(e) {
        console.log(e);
        this.reverseGeocoder(e.detail.markerId);
    },
    //逆地址解析
    reverseGeocoder: function (id) {
        var that = this;
        var location = "";
        var s = "markers[" + parseInt(id) + "]";
        var stationInfo = {};
        that.data.markers.forEach((item) => {
            if (item.id == id) {
                location = location + item.latitude + "," + item.longitude;
                stationInfo = item;
                return false;
            }
        });
        console.log(location);
        qqmapsdk.reverseGeocoder({
            //位置坐标，默认获取当前位置，非必须参数
            /**
             * 
             //Object格式
              location: {
                latitude: 39.984060,
                longitude: 116.307520
              },
            */
            /**
             *
             //String格式
              location: '39.984060,116.307520',
            */
            location: location || '', //获取表单传入的位置坐标,不填默认当前位置,示例为string格式
            success: function (res) { //成功后的回调
                console.log(res);
                stationInfo.title = res.result.formatted_addresses.recommend; //res.result.address
                stationInfo.callout.content = res.result.formatted_addresses.recommend;
                that.setData({
                    [s]: stationInfo
                });
                that.openLocation(stationInfo);
            },
            fail: function (error) {
                console.error(error);
            }
        });
    },
    //查看位置
    openLocation: function (stationInfo) {
        //使用微信内置地图查看位置接口
        wx.openLocation({
            latitude: stationInfo.latitude, // 纬度，浮点数，范围为90 ~ -90
            longitude: stationInfo.longitude, // 经度，浮点数，范围为180 ~ -180。
            name: stationInfo.title, //要找的地方名字（某某饭店）
            address: stationInfo.title, //地址：要去的地点详细描述
            scale: 14, // 地图缩放级别,整形值,范围从1~28。默认为最大
            infoUrl: '' // 在查看位置界面底部显示的超链接,可点击跳转（测试好像不可用）
        });
    },
 
    /**
     * 生命周期函数--监听页面加载
     */
    onLoad: function (options) {
        this.getUserLocation();//用户授权获取地理信息，可以写在onShow里进行判断是否授权获取地理信息
        this.data.markers.forEach((item, i) => {
            //高德地图、腾讯地图以及谷歌中国区地图使用的是GCJ-02坐标系
            let gcj02tobd09 = zhuan_dingwei.wgs84togcj02(item.longitude, item.latitude);
            console.log(i, gcj02tobd09);
            item.longitude = gcj02tobd09[0];
            item.latitude = gcj02tobd09[1];
        });
        console.log(this.data.markers);
    },
 
    /**
     * 生命周期函数--监听页面初次渲染完成
     */
    onReady: function () {
 
    },
 
    /**
     * 生命周期函数--监听页面显示
     */
    onShow: function () {
 
    },
 
    /**
     * 生命周期函数--监听页面隐藏
     */
    onHide: function () {
 
    },
 
    /**
     * 生命周期函数--监听页面卸载
     */
    onUnload: function () {
 
    },
 
    /**
     * 页面相关事件处理函数--监听用户下拉动作
     */
    onPullDownRefresh: function () {
 
    },
 
    /**
     * 页面上拉触底事件的处理函数
     */
    onReachBottom: function () {
 
    },
 
    /**
     * 用户点击右上角分享
     */
    onShareAppMessage: function () {
 
    }
})
