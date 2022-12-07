plugins {
    id("com.android.application")
}

android {
    namespace = "com.zeromake.onscripter"
    compileSdk = 33
    defaultConfig {
        applicationId = "com.zeromake.onscripter"
        minSdk = 16
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"
    }
    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    sourceSets {
        getByName("main") {
            // armeabi-v7a,arm64-v8a,x86_64
            jniLibs.srcDirs("libs")
        }
    }
}

dependencies {
}
