pipeline {
  agent any
  stages {
    stage('Setup') {
      steps {
        sh 'git submodules update --init --recursive'
        sh 'mkdir build'
        dir(path: 'build') {
          sh 'cmake CODE_COVERAGE=ON ..'
        }

      }
    }

    stage('Build') {
      steps {
        dir(path: 'build') {
          sh 'make'
        }

      }
    }

    stage('Save Executable') {
      steps {
        archiveArtifacts(artifacts: 'build/src/Minecraft-Daemon', caseSensitive: true)
      }
    }

    stage('Test') {
      steps {
        sh 'ctest'
        sh './get_code_cov.sh'
        sh 'curl https://codecov.io/bash -o codecov.sh && chmod a+x codecov.sh && /bin/bash codecov.sh'
      }
    }

  }
  environment {
    CODECOV_TOKEN = '5f5c6ae9-85f2-4a63-8219-69fef3151803'
    COVERAGE_BUILD = 'true'
  }
}