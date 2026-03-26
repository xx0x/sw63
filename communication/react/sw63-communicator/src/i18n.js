import i18n from 'i18next'
import { initReactI18next } from 'react-i18next'
import LanguageDetector from 'i18next-browser-languagedetector'

import en from './locales/en.json'
import cs from './locales/cs.json'
import de from './locales/de.json'
import pl from './locales/pl.json'
import nl from './locales/nl.json'
import uk from './locales/uk.json'
import hu from './locales/hu.json'

i18n
    .use(LanguageDetector)
    .use(initReactI18next)
    .init({
        resources: {
            en: { translation: en },
            cs: { translation: cs },
            de: { translation: de },
            pl: { translation: pl },
            nl: { translation: nl },
            uk: { translation: uk },
            hu: { translation: hu }
        },
        fallbackLng: 'en',
        supportedLngs: ['en', 'cs', 'de', 'pl', 'nl', 'uk', 'hu'],
        detection: {
            order: ['localStorage', 'navigator'],
            caches: ['localStorage'],
            lookupLocalStorage: 'sw63_language'
        },
        interpolation: {
            escapeValue: false
        }
    })


export const languageOptions = [
    { value: 'cs', label: 'Čeština' },
    { value: 'de', label: 'Deutsch' },
    { value: 'en', label: 'English' },
    { value: 'hu', label: 'Magyar' },
    { value: 'nl', label: 'Nederlands' },
    { value: 'pl', label: 'Polski' },
    { value: 'uk', label: 'Українська' }
];


export default i18n
