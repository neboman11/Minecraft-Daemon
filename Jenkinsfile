pipeline {
  agent any
  stages {
    stage('Setup') {
      steps {
        sh './autogen.sh'
      }
    }

    stage('Build') {
      steps {
        sh 'make'
      }
    }

    stage('Save Executable') {
      steps {
        archiveArtifacts(artifacts: 'Minecraft-Daemon', caseSensitive: true)
      }
    }

    stage('Test') {
      steps {
        sh 'curl https://codecov.io/bash -o codecov.sh && chmod a+x codecov.sh && /bin/bash codecov.sh'
      }
    }

  }
  environment {
    CODECOV_TOKEN = '5f5c6ae9-85f2-4a63-8219-69fef3151803'
  }
}