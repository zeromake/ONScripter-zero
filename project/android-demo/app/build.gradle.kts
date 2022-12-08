plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.zeromake.onscripter"
    compileSdk = 33

    defaultConfig {
        applicationId = "com.zeromake.onscripter"
        minSdk = 22
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
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
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        viewBinding = true
    }
    sourceSets {
        getByName("main") {
            // armeabi-v7a,arm64-v8a,x86_64
            jniLibs.srcDirs("libs")
        }
    }
}

dependencies {
     implementation(project(":sdl"))

     implementation("androidx.core:core-ktx:1.9.0")
     implementation("androidx.appcompat:appcompat:1.5.1")
     implementation("com.google.android.material:material:1.7.0")
     implementation("androidx.constraintlayout:constraintlayout:2.1.4")
     implementation("androidx.navigation:navigation-fragment-ktx:2.5.3")
     implementation("androidx.navigation:navigation-ui-ktx:2.5.3")

    testImplementation("junit:junit:4.13.2")

    androidTestImplementation("androidx.test.ext:junit:1.1.4")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.0")
}