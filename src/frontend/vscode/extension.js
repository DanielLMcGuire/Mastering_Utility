const { workspace, ExtensionContext } = require('vscode');
const { LanguageClient } = require('vscode-languageclient/node');

function activate(context) {
    console.log("Activated");

    const clientOptions = {
        documentSelector: [{ scheme: 'file', language: 'mas' }],
        synchronize: {
            fileEvents: workspace.createFileSystemWatcher('**/*.mas')
        }
    };

    const client = new LanguageClient('mas', 'Mastering Utility Language Server', clientOptions);

    context.subscriptions.push(client.start());
}

function deactivate() {
    return undefined;
}

module.exports = { activate, deactivate };
