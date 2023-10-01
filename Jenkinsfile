pipeline {
    agent none
    stages {
        stage('Build') {
            agent {
                dockerfile {
                    filename 'Dockerfile'
                    dir '.'
                    additionalBuildArgs '--target esp_base'
                }
            }
            steps {
            //checkout scm

                sh(label: 'Building', script: """
                    echo $PWD
                    . $HOME/esp/esp-idf/export.sh
                    idf.py build
                """)    
                // sh ". $HOME/esp/esp-idf/export.sh"    
                // sh "idf.py build"
            }            
        }
    }
}

    