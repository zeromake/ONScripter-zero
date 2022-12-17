plugins {
    id("com.android.application")
//    id("org.jetbrains.kotlin.android")
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
    implementation(project(":XXPermissions"))
    implementation("com.android.support:support-annotations:28.0.0")
//    implementation("androidx.core:core:1.9.0")
//    implementation("com.squareup.picasso:picasso:2.8")
    implementation("com.nostra13.universalimageloader:universal-image-loader:1.9.5")
//    implementation("com.facebook.fresco:fresco:2.6.0")
}
