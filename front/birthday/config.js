const config = {
    // 安全验证 (日期解锁)
    unlockDate: {
        month: 12,
        day: 23
    },

    // 1. 信封页
    envelope: {
        title: "To: 最亲爱的妈妈",
        subTitle: "点击开启这份特别的礼物",
        hint: "请输入您的生日日期开启"
    },

    // 2. 蛋糕页
    cake: {
        title: "Happy Birthday!",
        titleCn: "亲爱的母亲，生日快乐",
        message: [
            "愿时光温柔，岁月静好",
            "愿您永远被爱包围",
            "愿快乐与健康常伴左右"
        ],
        interactionHint: "✨ 点击屏幕吹灭蜡烛许愿 ✨"
    },

    // 3. 许愿页
    wish: {
        text: "闭上眼睛，许下一个美好的愿望...",
        subText: "愿所有的梦想都能照进现实"
    },

    // 3.5 幻灯片页 (温馨回忆)
    slideshow: {
        title: "岁月如歌",
        messages: [
            "还记得这些美好的瞬间吗？",
            "每一个微笑都值得珍藏",
            "感谢您给予我生命中最温暖的光",
            "时光荏苒，爱意永恒",
            "您的怀抱永远是我最温暖的港湾",
            "岁月的痕迹是您爱的勋章",
            "愿这世界温柔待您，如您待我一般",
            "无论走多远，心总系着家和您",
            "感恩有您，让我的生命如此丰盈",
            "妈妈，您是我永远的骄傲"
        ],
        duration: 3000 // 每张幻灯片停留时间
    },

    // 4. 相册页 (沉浸式回忆)
    album: {
        title: "时光的礼物",
        subTitle: "每一张照片，都是爱的印记",
        // 使用本地图片路径
        images: [
            "assets/photos/ma1.jpg", 
            "assets/photos/ma2.jpg",
            "assets/photos/ma3.jpg",
            "assets/photos/ma5.jpg",
            "assets/photos/ma6.jpg",
            "assets/photos/ma7.jpg",
            "assets/photos/ma8.jpg",
            "assets/photos/ma9.jpg",
            "assets/photos/ma10.jpg",
            "assets/photos/maf.jpg"
        ],
        interval: 4000, // 图片切换间隔 (ms)
        transitionDuration: 1500 // 过渡时间 (ms)
    },

    // 5. 结尾页 (贺卡风格)
    ending: {
        title: "致最爱的老妈",
        poem: [
            "祝 愿时光多温柔",
            "步 履轻盈皆自由",
            "舞 动青春竞风流",
            "彬 彬有礼展宏图",
            "生 辰吉乐岁岁安",
            "日 月同辉映笑颜",
            "快 意人生多喜乐",
            "乐 享今朝福满天"
        ],
        cardBody: [
            "妈妈，今天是属于您的节日。",
            "感谢您多年的辛勤付出与无私关爱。",
            "在这个特别的日子里，",
            "想对您说一声：您辛苦了！",
            "愿您今后的每一天都笑容灿烂，",
            "身体健康，万事顺遂！"
        ],
        signature: "爱您的孩子 敬上",
        finalButton: "再次回味"
    },
    
    // 音乐
    // 已替换为《告白之夜》
    bgm: "assets/bgm.mp3"
};
