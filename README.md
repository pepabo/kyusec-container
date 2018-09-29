Container for Kyushu Security Conference
==

[九州セキュリティカンファレンス2018](https://kyusec.student-kyushu.org/) で使用するハンズオン資料です

## 講義の前に

### 1. 本リポジトリをチェックアウトしてください

* https://github.com/pepabo/kyusec-container.git

### 2. VM のセットアップ

チェックアウト先の root で vagrant up を実行してください。

#### もし、 ダウンロードが遅かったり、 vagrant up で起動しない場合は:

Vagrant box を以下のURLからダウンロードしてください。

* https://drive.google.com/file/d/1fI_kYmKaYZKmv8djCaE3IagiCkbQsmK1/view?usp=sharing

その後、以下の手順を実行してみてください。

```bash
vagrant box add kyusec-container /path/to/package.box
vagrant init kyusec-container
vagrant up
```
