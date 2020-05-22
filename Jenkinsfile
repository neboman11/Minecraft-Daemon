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
    CODECOV_TOKEN = 'bdf4f94d-d854-4e61-bd40-aa5d666f3868'
  }
}