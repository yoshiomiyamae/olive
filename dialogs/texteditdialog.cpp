/***

    Olive - Non-Linear Video Editor
    Copyright (C) 2019  Olive Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#include "texteditdialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QIcon>
#include <QTimer>
#include <QDebug>

#include "ui/icons.h"

TextEditDialog::TextEditDialog(QWidget *parent, const QString &s, bool rich_text) :
  QDialog(parent),
  rich_text_(rich_text)
{
  setWindowTitle(tr("Edit Text"));

  QVBoxLayout* layout = new QVBoxLayout(this);

  // Create central text editor object
  textEdit = new QTextEdit(this);
  textEdit->setUndoRedoEnabled(true);

  // Upper toolbar
  if (rich_text) {
    QHBoxLayout* toolbar = new QHBoxLayout();

    // Bold Button
    /*
    bold_button = new QPushButton();
    bold_button->setIcon(QIcon(":/icons/bold.svg"));
    bold_button->setCheckable(true);
    connect(bold_button, SIGNAL(clicked(bool)), this, SLOT(SetBold(bool)));
    toolbar->addWidget(bold_button);
    */

    // Italic Button
    italic_button = new QPushButton();
    italic_button->setIcon(QIcon(":/icons/italic.svg"));
    italic_button->setCheckable(true);
    connect(italic_button, SIGNAL(clicked(bool)), textEdit, SLOT(setFontItalic(bool)));
    toolbar->addWidget(italic_button);

    // Underline Button
    underline_button = new QPushButton();
    underline_button->setIcon(QIcon(":/icons/underline.svg"));
    underline_button->setCheckable(true);
    connect(underline_button, SIGNAL(clicked(bool)), textEdit, SLOT(setFontUnderline(bool)));
    toolbar->addWidget(underline_button);

    // Font Name
    font_list = new QFontComboBox();
    connect(font_list, SIGNAL(currentIndexChanged(const QString&)), textEdit, SLOT(setFontFamily(const QString&)));
    toolbar->addWidget(font_list);

    // Font Weight
    font_weight = new QComboBox();
    font_weight->addItem(tr("Thin"), QFont::Thin);
    font_weight->addItem(tr("Extra Light"), QFont::ExtraLight);
    font_weight->addItem(tr("Light"), QFont::Light);
    font_weight->addItem(tr("Normal"), QFont::Normal);
    font_weight->addItem(tr("Medium"), QFont::Medium);
    font_weight->addItem(tr("Demi Bold"), QFont::DemiBold);
    font_weight->addItem(tr("Bold"), QFont::Bold);
    font_weight->addItem(tr("Extra Bold"), QFont::ExtraBold);
    font_weight->addItem(tr("Black"), QFont::Black);
    connect(font_weight, SIGNAL(currentIndexChanged(int)), this, SLOT(SetFontWeight(int)));
    toolbar->addWidget(font_weight);

    // Font Size
    font_size = new LabelSlider();
    connect(font_size, SIGNAL(valueChanged(double)), textEdit, SLOT(setFontPointSize(qreal)));
    toolbar->addWidget(font_size);

    // Font Color
    font_color = new ColorButton();
    connect(font_color, SIGNAL(color_changed(const QColor&)), textEdit, SLOT(setTextColor(const QColor &)));
    toolbar->addWidget(font_color);

    toolbar->addStretch();

    // Left Align
    left_align_button = new QPushButton();
    left_align_button->setIcon(QIcon(":/icons/align-left.svg"));
    left_align_button->setCheckable(true);
    left_align_button->setProperty("a", Qt::AlignLeft);
    connect(left_align_button, SIGNAL(clicked(bool)), this, SLOT(SetAlignmentFromProperty()));
    toolbar->addWidget(left_align_button);

    // Center Align
    center_align_button = new QPushButton();
    center_align_button->setIcon(QIcon(":/icons/align-center.svg"));
    center_align_button->setCheckable(true);
    center_align_button->setProperty("a", Qt::AlignCenter);
    connect(center_align_button, SIGNAL(clicked(bool)), this, SLOT(SetAlignmentFromProperty()));
    toolbar->addWidget(center_align_button);

    // Right Align
    right_align_button = new QPushButton();
    right_align_button->setIcon(QIcon(":/icons/align-right.svg"));
    right_align_button->setCheckable(true);
    right_align_button->setProperty("a", Qt::AlignRight);
    connect(right_align_button, SIGNAL(clicked(bool)), this, SLOT(SetAlignmentFromProperty()));
    toolbar->addWidget(right_align_button);

    // Justify Align
    justify_align_button = new QPushButton();
    justify_align_button->setIcon(QIcon(":/icons/justify-center.svg"));
    justify_align_button->setCheckable(true);
    justify_align_button->setProperty("a", Qt::AlignJustify);
    connect(justify_align_button, SIGNAL(clicked(bool)), this, SLOT(SetAlignmentFromProperty()));
    toolbar->addWidget(justify_align_button);

    layout->addLayout(toolbar);
  }

  layout->addWidget(textEdit);

  // Lower toolbar
  /*
  if (rich_text) {
    QHBoxLayout* lower_toolbar = new QHBoxLayout();

    lower_toolbar->addWidget(new QLabel(tr("Letter Spacing:")));

    letter_spacing = new LabelSlider();
    connect(letter_spacing, SIGNAL(valueChanged(double)), this, SLOT(SetLetterSpacing(qreal)));
    lower_toolbar->addWidget(letter_spacing);

    lower_toolbar->addStretch();

    layout->addLayout(lower_toolbar);
  }
  */

  // Create dialog buttons at the bottom
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  buttons->setCenterButtons(true);
  layout->addWidget(buttons);
  connect(buttons, SIGNAL(accepted()), this, SLOT(save()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(cancel()));

  connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(UpdateUIFromTextCursor()));
  if (rich_text_) {
    textEdit->setHtml(s);
  } else {
    textEdit->setPlainText(s);
  }

  // Helps ensure the UI elements update correctly at the beginning - when the cursor is at the start, the UI elements
  // show up blank...
  textEdit->moveCursor(QTextCursor::End);
}

const QString& TextEditDialog::get_string() {
  return result_str;
}

void TextEditDialog::save() {
  result_str = rich_text_ ? textEdit->toHtml() : textEdit->toPlainText();
  accept();
}

void TextEditDialog::cancel() {
  reject();
}

void TextEditDialog::SetBold(bool bold)
{
  QFont f = textEdit->currentFont();
  f.setBold(bold);
  textEdit->setCurrentFont(f);
  UpdateUIFromTextCursor();
}

void TextEditDialog::SetFontWeight(int i)
{
  textEdit->setFontWeight(font_weight->itemData(i).toInt());
}

void TextEditDialog::SetLetterSpacing(qreal spacing)
{
  QFont f = textEdit->currentFont();
  f.setLetterSpacing(f.letterSpacingType(), spacing);
  textEdit->setCurrentFont(f);
}

void TextEditDialog::SetAlignmentFromProperty()
{
  textEdit->setAlignment(static_cast<Qt::Alignment>(sender()->property("a").toInt()));
  UpdateUIFromTextCursor();
}

void TextEditDialog::UpdateUIFromTextCursor()
{
  italic_button->setChecked(textEdit->fontItalic());
  underline_button->setChecked(textEdit->fontUnderline());
  font_list->setCurrentText(textEdit->fontFamily());
  font_size->SetValue(textEdit->fontPointSize());
  font_color->set_color(textEdit->textColor());

  for (int i=0;i<font_weight->count();i++) {
    if (font_weight->itemData(i).toInt() == textEdit->fontWeight()) {
      font_weight->blockSignals(true);
      font_weight->setCurrentIndex(i);
      font_weight->blockSignals(false);
      break;
    }
  }

  Qt::Alignment align = textEdit->alignment();
  left_align_button->setChecked(align == Qt::AlignLeft);
  center_align_button->setChecked(align == Qt::AlignCenter);
  right_align_button->setChecked(align == Qt::AlignRight);
  justify_align_button->setChecked(align == Qt::AlignJustify);
}
